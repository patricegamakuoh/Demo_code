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
 *            @file:    atl_encoder.c 
 *          @author:    Patrice Gama
 *            @date:    2022-02-21
 *       @Component:    atl_encoder_knob
 *              
 *     @Description:    The atl_encoder reads the rotary encoder value via the ISR
 *    service and add an ISR handler function to detect the A pin's rising and 
 *    falling edges and determine the direction of rotation based on the B pin's 
 *    level. Finally, the current encoder are counted every seconds.
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"

#include "atl_app_hardware.h"


volatile int32_t encoder_count = 0;

void encoder_isr_handler(void *arg)
{
    static uint8_t prev_a_level = 0;
    uint8_t curr_a_level = gpio_get_level(SW_A_PIN);
    uint8_t curr_b_level = gpio_get_level(SW_B_PIN);

    if ((prev_a_level == 0) && (curr_a_level == 1))
    {
        // A rising edge, check B level to determine direction
        if (curr_b_level == 0)
        {
            encoder_count++;
        }
        else
        {
            encoder_count--;
        }
    }
    else if ((prev_a_level == 1) && (curr_a_level == 0))
    {
        // A falling edge, check B level to determine direction
        if (curr_b_level == 0)
        {
            encoder_count--;
        }
        else
        {
            encoder_count++;
        }
    }
    prev_a_level = curr_a_level;
}

void atl_encoder_init(void)
{
    gpio_config_t encoder_gpio_conf = {
        .pin_bit_mask = (1ULL << SW_A_PIN) | (1ULL << SW_B_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    gpio_config(&encoder_gpio_conf);

    // Configure PCNT unit
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = SW_A_PIN,
        .ctrl_gpio_num = SW_B_PIN,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DEC,
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_REVERSE,
        .counter_h_lim = 100,
        .counter_l_lim = 0
    };

    pcnt_unit_config(&pcnt_config);

    // Install ISR service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(SW_A_PIN, encoder_isr_handler, NULL);
}

int32_t readEncoder_count(void){
    return encoder_count;
}






