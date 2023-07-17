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
 *            @file:    atl_max6675.c    
 *          @author:    Patirce Gama
 *            @date:    2023-04-7
 *       @Component:    atl_max6675_ADC
 *              
 *     @Description:    The MAX6675 is a 12-bit analog to digital converter (ADC)
 *    for the typ-K thermocouple, data is read via the SPI interface and the 
 *    temperature range is from 0*C to 1024*C
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#include "atl_max6675.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "esp_rom_gpio.h"

void delay_us(uint32_t us)
{
    uint64_t start_time = esp_timer_get_time();
    while ((esp_timer_get_time() - start_time) < us);
}

MAX6675_t *max6675;
  
MAX6675_t *atl_MAX6675_init(gpio_num_t cs, gpio_num_t sclk, gpio_num_t miso)
{
    max6675 = (MAX6675_t *)malloc(sizeof(MAX6675_t));
    if (max6675 == NULL) {
        return NULL;
    }
    max6675->cs = cs;
    max6675->sclk = sclk;
    max6675->miso = miso;

    printf("wait initializing MAX... \n"); /*print the inititalizer one time*/
    esp_rom_gpio_pad_select_gpio(cs);  
    gpio_set_direction(cs, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(sclk);
    gpio_set_direction(sclk, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(miso);
    gpio_set_direction(miso, GPIO_MODE_INPUT);
    gpio_set_level(cs, 1);

    /*wait for MAX chip to stabilize*/
    return max6675;
}

float MAX6675_readCelsius(MAX6675_t *max6675)
{
    uint16_t v;

    gpio_set_level(max6675->cs, 0);
    delay_us(10); /*Delay for 10 microseconds*/ 

    v = MAX6675_spiread(max6675);
    v <<= 8;
    v |= MAX6675_spiread(max6675);

    gpio_set_level(max6675->cs, 1);

    if (v & 0x4) {
        /*uh oh, no thermocouple attached!*/ 
        return 00.1;
        // return -100;
    }

    v >>= 3;
    return v * 0.25;

}

float MAX6675_readFahrenheit(MAX6675_t *max6675)
{
    return MAX6675_readCelsius(max6675) * 9.0 / 5.0 + 32;
}

uint8_t MAX6675_spiread(MAX6675_t *max6675)
{
    int i;
    uint8_t d = 0;

    for (i = 7; i >= 0; i--) {
        gpio_set_level(max6675->sclk, 0);
        delay_us(10);
        if (gpio_get_level(max6675->miso)) {
    /*set the bit to 0 no matter what*/ 
            d |= (1 << i);
        }
        gpio_set_level(max6675->sclk, 1);
        delay_us(10);
    }

   /*wait for max chip to settle or delay to read sensor data */
   vTaskDelay(100/portTICK_PERIOD_MS); 

    return d;
}
