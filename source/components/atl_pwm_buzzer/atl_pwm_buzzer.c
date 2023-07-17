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
 *            @file:    atl_speaker.c    
 *          @author:    Patirce Gama
 *            @date:    2023-04-7
 *       @Component:    atl_speaker
 *              
 *     @Description:    The atl speaker (buzzer) provide's the beep sounds for the 
 *    controller, the speaker name is used with the intension that the buzzer 
 *    sound can later be change to real human like voise . 
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
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#include "atl_pwm_buzzer.h"
#include "atl_app_hardware.h"

#define BUZZER_CHANNEL LEDC_CHANNEL_0
#define BUZZER_FREQ_HZ 2000
#define BUZZER_DUTY_CYCLE 180  /*Buzzer sound*/

typedef struct {
    int beep_count;
    int beep_duration_ms;
} buzzer_config_t;

/*1 second interval between buzzer_on and buzzer_off this 
 *this should later be change to alt define timer count*/
// #define TIMER_INTERVAL_US 1000000 

void atl_speaker_init(void)
{
    /*Configure speaker pin as output*/ 
    esp_rom_gpio_pad_select_gpio(SPEAKER_PIN);
    gpio_set_direction(SPEAKER_PIN, GPIO_MODE_OUTPUT);

    /*Configure LEDC timer and channel for sperker PWM*/ 
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = BUZZER_FREQ_HZ,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .channel = BUZZER_CHANNEL,
        .duty = 0,
        .gpio_num = SPEAKER_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&channel_conf);
}

void buzzer_sound(int count, int duration_ms, int timer_interval) {
        /*Create and start timer*/ 
        buzzer_config_t buzzer_config = {
            .beep_count = count,
            .beep_duration_ms = duration_ms,
        };
        esp_timer_handle_t buzzer_timer;
        esp_timer_create_args_t buzzer_timer_args = {
            .callback = buzzer_timer_callback,
            .arg = &buzzer_config,
            .name = "buzzer_timer"
        };
        esp_timer_create(&buzzer_timer_args, &buzzer_timer);
        esp_timer_start_periodic(buzzer_timer, timer_interval);

        /*Wait for beeping to finish, The vTaskDelay call inside the while loop is
         *used to prevent the loop from consuming too much CPU*/ 
        while (buzzer_config.beep_count > 0) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        /*Stop timer and turn off buzzer sound*/ 
        esp_timer_stop(buzzer_timer);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL);
    }

 void buzzer_timer_callback(void* arg, int val)
{
    static bool buzzer_on = false;
    buzzer_config_t* buzzer_config = (buzzer_config_t*)arg;

    if (buzzer_on) {
        /*Turn off speaker sound*/ 
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL);
        buzzer_on = false;
    } else {
        /*Turn on speaker sound*/ 
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL, val);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL);
        buzzer_on = true;
    }

    /*Decrement beep count and stop timer if necessary*/ 
    buzzer_config->beep_count--;
    if (buzzer_config->beep_count == 1) {
        esp_timer_stop((esp_timer_handle_t) arg);
    }
}