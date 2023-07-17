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
 *            @file:    atl_max6675.h     
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

#ifndef MAX6675_H
#define MAX6675_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"

typedef struct {
    // struct spi_device_t *spi;
    gpio_num_t cs;
    gpio_num_t sclk;
    gpio_num_t miso;
} MAX6675_t;

/*initialize the MAX6675 ADC*/
MAX6675_t *atl_MAX6675_init(gpio_num_t cs, gpio_num_t sclk, gpio_num_t miso);
/*read the temperature in celsius from MAX6675 ADC*/
float MAX6675_readCelsius(MAX6675_t *max6675);
/*read the temperature in Fahrenheit from MAX6675 ADC*/
float MAX6675_readFahrenheit(MAX6675_t *max6675);
/*Read the SPI bytes data*/
uint8_t MAX6675_spiread(MAX6675_t *max6675);

#endif


