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
 *            @file:    atl_motor_control.h  
 *          @author:    Patrice Gama
 *            @date:    2023-12-4
 *       @Component:    atl_motor_control
 *              
 *     @Description:    The atl_motor_control provides the PWM signal requirements 
 *    for the grill Fan control. There are two API for PWM signal from esp idf 
 *    the LEDC and MCPWM API, the MCPWM are much preferred for high speed (200MHz)
 *    while the LEDC is preferable for low frequency applications (80MHz). for atl
 *    application, the MCPWM is a better choice as it can provide maximum airflow 
 *    to the grill system. 
 *      
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "driver/ledc.h"
#include "driver/mcpwm.h"
// #include "driver/mcpwm_prelude.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "atl_motor_driver.h"
#include "atl_FPID.h"
#include "atl_app_hardware.h"


/*optional use of LEDC for motor driver*/
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (2)
#define LEDC_TEST_DUTY         (8192)
#define LEDC_TEST_FADE_TIME    (3000)
#define LEDC_TEST_DUTY1         (400)


ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM]={
    {
        .channel    = LEDC_LS_CH3_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH0_GPIO,
        .speed_mode = LEDC_LS_MODE,
        .timer_sel  = LEDC_LS_TIMER
    },
};

void fan_pwmc_init(void)
{
	int ch;
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_14_BIT, /*resolution of PWM duty*/ 
        .freq_hz = 4000,                      /*frequency of PWM signal*/ 
        .speed_mode = LEDC_LS_MODE,           /*timer mode*/ 
        .timer_num = LEDC_LS_TIMER,           /*timer index*/ 
        .clk_cfg = LEDC_AUTO_CLK,             /*Auto select the source clock*/ 
    };

    /*Set configuration of timer0 for high speed channels*/
    ledc_timer_config(&ledc_timer);
#ifdef CONFIG_IDF_TARGET_ESP32
    /*Prepare and set configuration of timer1 for low speed channels*/ 
    ledc_timer.speed_mode = LEDC_HS_MODE;
    ledc_timer.timer_num = LEDC_HS_TIMER;
    ledc_timer_config(&ledc_timer);
#endif
    /*Set LED Controller with previously prepared configuration*/ 
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }
}

void atl_fan_init(void){
    /*Configure MCPWM module for DC motor control*/ 
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;               /*Set PWM frequency to 1 kHz*/ 
    pwm_config.cmpr_a = 0;                     /*Set initial duty cycle to 0*/ 
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    /*Control motor speed and direction*/ 
}

uint16_t write_PWM(float PWM_dutyCycle){
    return mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, PWM_dutyCycle);
}