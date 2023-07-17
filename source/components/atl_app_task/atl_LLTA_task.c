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
 *            @date:    2023-03-26
 *       @Component:    atl_app_task
 *              
 *     @Description:    The atl_app_task runs the real-time events (rtos) for the 
 *    whole application, prioritize task and meet strict timing requirements 
 *    for the app.
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
#include "driver/uart.h"

#include "atl_task_scheduler.h"
#include "atl_LLTA_task.h"

#include "atl_app_hardware.h"
#include "atl_app_defines.h"
#include "atl_uart.h"
#include "atl_max6675.h"
#include "atl_motor_driver.h"
#include "atl_encoder.h"
#include "atl_XSH28.h"
#include "atl_pwm_buzzer.h"

static const char *TAG = "LLTA";


/*Task defincation task test */

void atl_LLTA_task(void *pvParameter);

/*Print LLTA info notification to the serial terminal*/
void print_intro_notice(void);

char stringtype[80];
void *arg;
static const int RX_BUF = 1024;

void app_create_LLTA_task(void)
{   

    /*Define Windows uart task*/
    static StaticTask_t xTaskBuffer2;
    static StackType_t xStack2[LLTA_TASK_STACK_SIZE];
    /*Create the task pinned to defined core without using any dynamic memory allocation.*/ 
    TaskHandle_t xHandle2 = xTaskCreateStaticPinnedToCore(
                   atl_LLTA_task,         /*Function that implements the task.*/
                   "atl_LLTA_task",       /*Text name for the task.*/
                   LLTA_TASK_STACK_SIZE,  /*Stack size in bytes, not words.*/ 
                   (void*)1,                  /*Parameter passed into the task.*/
                   LLTA_TASK_PRIORITY,    /*Priority at which the task is created.*/ 
                   xStack2,                   /*Array to use as the task's stack.*/ 
                   &xTaskBuffer2,             /*Variable to hold the task's data structure.*/ 
                   LLTA_TASK_CORE_ID );   /*Specify the task's core affinity*/ 
    /*Check if task was created successfully*/
    if (xHandle2 == NULL) {
        printf("Failed to create atl_LLTA_task \n");
        return;
    }
    vTaskSuspend(xHandle2);    /*Suspend the task immediately after creating it*/ 
    vTaskResume(xHandle2);     /*Resume the task later when needed*/ 

}


void atl_LLTA_task(void *pvParameter)
{   
    /*initialize task begine*/

    atl_init_WIN_uart();     /*initialize atl_configured uart*/
    atl_fan_init();          /*initialize atl_MCPWM motor*/
    atl_encoder_init();      /*initialize alt_encoder*/
    atl_XSH28_ADC_init();    /*initialize alt_XSH28_ADC*/
    atl_speaker_init();      /*initialize alt_speaker sound*/

    print_intro_notice();
    
    /*initialize alt_speaker sound*/
    MAX6675_t *max6675 = atl_MAX6675_init(CS_GPIO, SCLK_GPIO, MISO_GPIO);
    static bool loop_print = false;

    /*initialize uart for the serial simulator*/
    uint8_t *data=(uint8_t*) malloc(RX_BUF);

    /*declare hardware function test key*/
    bool check_encoder = false;
    bool check_motor_driver = false;
    bool check_XSH28_ADC = false;
    bool check_max6675_ADC = false;
    bool check_cs1238_ADC = false;
    bool check_speaker_sound = false;
    bool check_ili9488_LCD = false;
    bool check_aws_IoT = false;

   /*End init*/

    while (1) {

        int len = uart_read_bytes(UART_NUM_0, data, 
        (RX_BUF - 1), 20 / portTICK_PERIOD_MS);

        /*check if user wants to quit*/
        if (data[0] == 'x' &&! loop_print){
            print_intro_notice();
            write_PWM(0);    /*stop motor*/
            check_encoder = false;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            check_aws_IoT = false;
            loop_print=true;
        }

        /*Check user input*/
        if (data[0] == 'b' && loop_print){
            check_encoder = true;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            check_aws_IoT = false; 
            loop_print = false;    
        }
        if (data[0] == 'c' && loop_print){
            check_motor_driver = true;
            check_encoder = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            check_aws_IoT = false;
            loop_print=false;     
        }
        if (data[0] == 'd' && loop_print){
            check_XSH28_ADC = true;
            check_motor_driver = false;
            check_encoder = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            check_aws_IoT = false;
            loop_print=false;      
        }
        if (data[0] == 'e' && loop_print){
            check_max6675_ADC = true; 
            check_encoder = false;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            check_aws_IoT = false;
            loop_print=false;    
        }
        if (data[0] == 'f' && loop_print){
            check_speaker_sound = true;
            check_encoder = false;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            check_aws_IoT = false;
            loop_print=false;     
        }
        if (data[0] == 'g' && loop_print){
            check_cs1238_ADC = true;
            check_encoder = false;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_aws_IoT = false; 
            loop_print=false;    
        }
        if (data[0] == 'h' && loop_print){
            check_ili9488_LCD= true; 
            check_encoder = false;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_aws_IoT = false;
            loop_print=false;    
        }
        if (data[0] == 'i' && loop_print){
            check_aws_IoT = true; 
            check_encoder = false;
            check_motor_driver = false;
            check_XSH28_ADC = false;
            check_max6675_ADC = false;
            check_speaker_sound = false;
            check_cs1238_ADC = false;
            check_ili9488_LCD = false;
            loop_print=false;   
        }

        /*Exercute functions*/
        if (check_encoder){
            printf("\n turn the knob to see encoder value = %ld \n", readEncoder_count());
        }
        if (check_motor_driver){
            float uart_setpoint = atoi(stringtype); 
            if (!len){
                printf("\n please input fan speed 0~100 >>>>> %f \n", uart_setpoint);
            }
            if (len) {
                data[len] = '\0';
                sprintf(stringtype, "%s", (char*)data);
            }                   
            write_PWM(uart_setpoint);    /*write speed to motor*/
        }
        if (check_XSH28_ADC){
            receive_XSH28_ADC();
        }
        if (check_max6675_ADC){
            printf("Fahrenheit = %.2f Celcius = %.2f \n", MAX6675_readFahrenheit(max6675), 
            MAX6675_readCelsius(max6675));
        }
        if (check_speaker_sound){
            printf("\n wait for buzzer beep... !\n");
            /*input beep cound, beep duration (ms) and beep interval in us between off/on*/
            buzzer_sound(10, 1000, 90000);   
            buzzer_timer_callback(arg, 500);   /*input pwm duty cycle*/
        }
        if (check_cs1238_ADC){
            printf("\n\n\n\n\n The cs123 ADC is not available at moment ! \n\n\n\n\n");
        }
        if (check_ili9488_LCD){
            printf("\n\n\n\n\n The ili9488_LCD is not available at moment ! \n\n\n\n\n");
        }
        if (check_aws_IoT){
            printf("\n\n\n\n\n The AWS IoT is not available at moment ! \n\n\n\n\n");
        }
     }
     /*Cleanup*/ 
     uart_driver_delete(UART_NUM_0);
}
/*End tasks*/

/*Print intro information*/
void print_intro_notice(void){
    printf("\n");
    printf("==========================================================================\n");
    printf("                     ATL LLTA CONTROLLER TEST TOOL                        \n");
    printf("==========================================================================\n");
    printf("Enter the following characters via the UART Windows APP to check hardware \n");
    printf("\n");
    printf("b = check encoder knob values\n");
    printf("c = check motor driver\n");
    printf("d = check XSH28 (Buttons, MP1, MP2, MP3, MP4) ADC values\n");
    printf("e = check max6675 ADC\n");
    printf("f = check speaker sound\n");
    printf("g = check cs1238 ADC\n");
    printf("h = check ili9488 display\n");
    printf("i = chech AWS connectivity\n");
    printf("\n");
    printf(">>>>>>>> press x to exit test <<<<<<<<<<<< \n");
    printf("\n\n");
}
/*End function define*/