/********************************************************************************
 *    Copyright (c), 2023, Atlas IoT Co. Ltd. 
 *    ---------------------------------------------------------------------------                          
 *                      
 *    This software is the property of "Atlas IoT Co. Ltd and may 
 *    not be copied or reproduced otherwise than on to a single hard disk for 
 *    backup or archival purposes.  The source code is confidential  
 *    information and must not be disclosed to third parties or used without  
 *    the express written permission of Atlas IoT Co. Ltd.
 *    Copyright (c), 2023, Atlas IoT Co. Ltd. All rights reserved. 
 *    ----------------------------------------------------------------------------
 *    ----------------------------------------------------------------------------
 *    FILE DESCRIPTION
 *    ----------------------------------------------------------------------------
 *            @file:    atl_app_task.c     
 *          @author:    Patrice Gama 
 *            @date:    2023-06-06
 *       @Component:    atl_RPID_task
 *              
 *     @Description:    This task demonstrates the implementation of the Robust 
 *                   PID (RPID) controller algorithm for the Venom controller. The 
 *                   RPID controller is a type of RPID controller that is designed 
 *                   to be more robust to disturbances and noise.
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "atl_task_scheduler.h"
#include "atl_RPID_task.h"

#include "atl_app_hardware.h"
#include "atl_FPID.h"
#include "atl_app_defines.h"
#include "atl_uart.h"
#include "atl_max6675.h"
#include "atl_motor_driver.h"
#include "atl_XSH28.h"

/*First outer_RPID*/
#define ROBUST_PID_KP  6.0f   /* start quick */ 
#define ROBUST_PID_KI  11.0f  /* released fast */
#define ROBUST_PID_KD  10.0f  /* decrease speed as error reach zero */ 
#define DEADBAND 0.0f         /* Off==0 */
#define PID_LIM_MIN 0.0f      /* Limit for PWM */
#define PID_LIM_MAX 100       /* Limit for PWM */

/*Second Inner_RPID Declearation*/
#define ROBUST_PID_KPy  250      /* 200 of limit 5 */ 
#define ROBUST_PID_KIy  245      /* 200.0f limit 7... */ 
#define ROBUST_PID_KDy  235
#define DEADBANDy 0.0f           /* Off==0 */
#define PID_LIM_MINy 0           /* Limit for PWM. */
#define PID_LIM_MAXy 30          /* Limit for PWM. */


/*700F Target best PWM*/ 
#define PWM_MIN_MAX 0          
#define PWM_MAX_MAX 60           
/*600F Target best PWM*/ 
#define PWM_MIN_htemp_6_7 60            /* Limit for PWM. */
#define PWM_MAX_htemp_6_7 100           /* Limit for PWM. */ //good performance for target 700f  
/*500F Target best PWM  */  
#define PWM_MIN_htemp_4_5 40            /* Limit for PWM. */  
#define PWM_MAX_htemp_4_5 50            /* Limit for PWM. */
/*300F Target best PWM */  
#define PWM_MIN_lwtemp_1_2_3 30         /* Limit for PWM. */  /*NB for 30 bellow increase if error */ 
#define PWM_MAX_lwtemp_1_2_3 50         /* Limit for PWM. */  /*NB for 30 bellow increase if error */ 

#define tast2s 					"task size"
#define ESP_INTR_FLAG_DEFAULT 0

/*Define Variables we'll be connecting to for RPID*/

/*First RPID*/
robust_PID_t pid_ctx;
/*Second RPID*/
robust_PID_t pid_cty;

/*first PID*/
float Input;
int Output = 0;
float deadband_delta;

/*second PID*/ 
float Inputy;
int Outputy = 0;
float deadband_deltay;

float act_grill_temp=0;
uint16_t Controled_PWM_out;

/*Function Declaration*/
void atl_RPID_task(void *pvParameter);

void app_create_RPID_task(void)
{   
    /*Define task*/
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[RPID_TASK_STACK_SIZE];
    /*Create the task pinned to defined core without using any dynamic memory allocation.*/ 
    TaskHandle_t xHandle = xTaskCreateStaticPinnedToCore(
                   atl_RPID_task,              /*Function that implements the task.*/
                   "atl_RPID_task",            /*Text name for the task.*/
                   RPID_TASK_STACK_SIZE,       /*Stack size in bytes, not words.*/ 
                   (void*)1,                   /*Parameter passed into the task.*/
                   RPID_TASK_PRIORITY,         /*Priority at which the task is created.*/ 
                   xStack,                     /*Array to use as the task's stack.*/ 
                   &xTaskBuffer,               /*Variable to hold the task's data structure.*/ 
                   RPID_TASK_CORE_ID);         /*Specify the task's core affinity*/ 
    /*Check if task was created successfully*/
    if (xHandle == NULL) {
        printf("Failed to create RPID task \n");
        return;
    }
    /*Task was created successfully, so we can use its handle to control it*/ 
    vTaskSuspend(xHandle);   /*Suspend the task immediately after creating it*/ 
    vTaskResume(xHandle);    /*Resume the task later when needed*/ 
}

/*Tasks function definition*/

void atl_RPID_task(void *pvParameter)
{   
    /*initialize task begine*/
    
    /*Init Windows UART for Serial communication*/
    atl_init_WIN_uart();

    /*Init motor fan*/
    atl_fan_init();

    /*Init ADC for Pit probe*/
    atl_XSH28_ADC_init();

    /*Init the first RPID*/
    robust_PID_info_t robust_PID_err = robust_PID_init(&pid_ctx,act_grill_temp, 
    act_grill_temp, PID_LIM_MIN,ROBUST_PID_KP, ROBUST_PID_KI, ROBUST_PID_KD);

    if (robust_PID_err != ROBUST_PID_ERR_NONE) {
        printf("\n\n** ERROR: ROBUST_PID_err != ROBUST_PID_ERR_NONE **\n\n");
        while (1) { ; }
    }
    
    /*Init the second RPID*/
    robust_PID_info_t robust_PID_erry = robust_PID_init(&pid_cty,act_grill_temp, 
    act_grill_temp, PID_LIM_MINy,ROBUST_PID_KPy, ROBUST_PID_KIy, ROBUST_PID_KDy);

    if (robust_PID_erry != ROBUST_PID_ERR_NONE) {
        printf("\n\n** ERROR: ROBUST_PID_err != ROBUST_PID_ERR_NONE **\n\n");
        while (1) { ; }
    }

    /*End init*/

    while (1) {

        /*Begin task */ 

        /*Recieved the serial input set point from windows UART*/                         /*receive uart windows command*/    
        float uart_setpoint = atoi(read_win_uart());    

        /*Read the actual grill temperature */
        act_grill_temp = measure_Pit_probe_Fahrenheit();
        //printf(" P = %.02f \n", measure_Pit_probe_Fahrenheit());

        /*Calculate the first RPID*/ 
        robust_pid_calc(&pid_ctx, uart_setpoint, act_grill_temp);                         /*Calculate outer PID terms values*/
        deadband_delta = pid_ctx.p_term + pid_ctx.i_term + pid_ctx.d_term;                /*outer deadband*/
        
        /*Calculate the second RPID*/  
        robust_pid_calc(&pid_cty, uart_setpoint, act_grill_temp);                         /*Calculate inner robust_PID param values*/
		deadband_deltay = pid_cty.p_term + pid_cty.i_term + pid_cty.d_term;               /*inner dead band*/  

        /*check condition for the inner loop*/ 
		if ((deadband_delta != deadband_delta) || (fabsf(deadband_delta) >= DEADBAND)) {

			/*Compute new control signal output */
            robust_pid_sum(&pid_cty, PID_LIM_MINy, PID_LIM_MAXy);         /*outer RPID*/

            if (pid_ctx.error2<20&&act_grill_temp>550){
                robust_pid_sum(&pid_cty, PWM_MIN_MAX , PWM_MAX_MAX );     /**/ 
                //Controled_PWM_out = (int)lroundf(pid_cty.y_out);        /**/
            }

			Controled_PWM_out = (int)lroundf(pid_cty.y_out);               /**/
							
			if ((deadband_deltay != deadband_delta) || (fabsf(deadband_deltay) >= DEADBAND)){
				if (pid_ctx.error2>20){
					robust_pid_sum(&pid_ctx, PID_LIM_MIN, PID_LIM_MAX);    /**/ 
					Controled_PWM_out = (int)lroundf(pid_ctx.y_out);       /**/
					//printf("outer pid output = %d, error2 = %d \n ",Controled_PWM_out,(int)lroundf(pid_ctx.error2));
					}
					//printf("PWM_OUT = %d \n ",Controled_PWM_out);  PWM_MAX 
				}
			}

			/*check condition and apply control rule for deferent 3 tempperature levels*/ 
			if (pid_ctx.error2<6&&pid_ctx.error2>-3&&act_grill_temp<350){
				robust_pid_sum(&pid_cty, PWM_MIN_lwtemp_1_2_3,PWM_MAX_lwtemp_1_2_3);         /*RPID_1 (for lowest temp level 100F, 200F, 300F)*/ 
				Controled_PWM_out = (int)lroundf(pid_cty.y_out);                             /**/
			}				
			if (pid_ctx.error2<6&&pid_ctx.error2>-3&&act_grill_temp>349&&act_grill_temp<550){
			    robust_pid_sum(&pid_cty, PWM_MIN_htemp_4_5, PWM_MAX_htemp_4_5);              /*RPID_2 (for middle temp level 400F, 500F)*/
				Controled_PWM_out = (int)lroundf(pid_cty.y_out);                             /**/
			}
			if (pid_ctx.error2<6&&pid_ctx.error2>-3&&act_grill_temp>449){
			    robust_pid_sum(&pid_cty, PWM_MIN_htemp_6_7, PWM_MAX_htemp_6_7);              /*RPID_3 (for highest temp level 600F, 700F)*/ 
				Controled_PWM_out = (int)lroundf(pid_cty.y_out);                             /**/
			}
			/*End check*/ 

        /*Print out control results*/
        printf("error = %d, Actual_grill_temp = %0.2f, Grill_set_temp =  %f, Fan_PWM = %d ",(int)lroundf(pid_ctx.error2),act_grill_temp, uart_setpoint, Controled_PWM_out); //intcode print 
        printf(" optimization = %ld \n", actual_temp_quantification(act_grill_temp, uart_setpoint));

        /*Write the controled PWM to motor, input duty cycle speed from 0% to 100%*/
        write_PWM(Controled_PWM_out);

        //vTaskDelay(10/portTICK_PERIOD_MS);

        /*End task*/ 
    }
}