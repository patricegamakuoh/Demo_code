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
 *            @file:    atl_xsh28.c    
 *          @author:    Patirce Gama
 *            @date:    2023-04-7
 *       @Component:    atl_xsh28_ADC
 *              
 *     @Description:    The XSH28 is a 12-bit analog to digital converter (ADC)
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

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <math.h>

#include "atl_uart.h"
#include "atl_XSH28.h"

#define R0 1000.0 /*resistance pt1000 at 0*/

/*Defines for temperature measurement*/
#define ADC_MAX_VALUE 4085
#define PT1000_MIN_RESISTANCE 723.30
#define PT1000_MAX_RESISTANCE 2809.80
#define RESISTANCE_GAIN 391           /*Tune this parameter according to the PT1000 resistance table*/

const float A = 3.9083e-3;            /*A coefficient for PT1000*/ 
const float B = -5.775e-7;            /*B coefficient for PT1000*/

static const int RX_BUF_SIZE = 1024;

typedef struct {
    uint8_t rxi;
	uint8_t checkbyte;
	uint8_t head_byte;
    uint16_t data_buff;
    int rxBytes;
	float ADC_value; 
	uint8_t *data;
	int16_t meat_probe_tempMP1;
    int16_t meat_probe_tempMP2;
    int16_t meat_probe_tempMP3;

} rx_param;

uint16_t Knob_key_AD;

uint8_t close_blue=0;
uint16_t Knob_key_AD;
uint16_t ntc_AD;

rx_param param;

/*Function declaration*/
float measure_Temp_Fahrenheit(int16_t probe_ADC);
float measure_Temp_Celsius(int16_t probe_ADC);
void read_ADC_Venom(void);

void atl_XSH28_ADC_init(void) {

    init_adc_uart();  /*initialize the uart for XSH28_ADC*/
    param.rxi=0;
	param.checkbyte=0;
	param.head_byte=0;
    param.data_buff=0;
    param.rxBytes=0;

    param.data = (uint8_t*) malloc(RX_BUF_SIZE+1); 
}  

/*recieved earch adc value*/  
void receive_XSH28_ADC(void) {
    
	param.rxBytes = uart_read_bytes(UART_NUM_1, param.data, RX_BUF_SIZE, 10/portTICK_PERIOD_MS);

	/*uncomment the below for loop to receive the sender bytes, and ensure to comment out other 
	 *printf statements of the whole code to avoid any mixed-up prints
    */

	// printf("Received paylaod: ");
	// for (int i = 0; i < 17; i++) {
	// 	//printf("0x%02X ", param.data[i]);
	// 	printf("%d ", param.data[i]);
	// }
	// printf("\n");

if(param.rxBytes > 10) 	
{ 
	param.data[param.rxBytes] = 0;
	param.rxBytes=0;
	param.head_byte=0;

	for(param.rxi=0;param.rxi<50;param.rxi++)
		{
			if(param.data[param.rxi]==0xe1)
				{
					param.head_byte=param.rxi;
				    //printf("HeaD BYTE 1! %d\n",head_byte);
					break;
				}
		}
	for(param.rxi=param.head_byte;param.rxi<15+param.head_byte;param.rxi++)
		{
			param.checkbyte+=param.data[param.rxi];
			//printf("HeaD BYTE 2! %d\n",param.checkbyte);
		}		
	/*recieved earch adc value*/  
	if(param.data[param.head_byte]==0xe1&&param.checkbyte==param.data[15+param.head_byte])
		{
			param.data_buff=param.data[1+param.head_byte];
			param.data_buff=param.data_buff<<8;
			Knob_key_AD=param.data[2+param.head_byte]|param.data_buff;                   /*received button press */
			printf("Buttons = %d  ",param.data_buff);

			param.data_buff=param.data[3+param.head_byte];
			param.data_buff=param.data_buff<<8;
			param.meat_probe_tempMP1=param.data[4+param.head_byte]|param.data_buff;       /*received P probe*/
			// printf(" P = %d ",param.data_buff);                                        /*print P probe ADC values*/
             
            printf("P = %.2fC ", measure_Temp_Celsius(param.meat_probe_tempMP1));         /*print MP1 probe ADC values*/
            printf("P = %.2fF   ", measure_Temp_Fahrenheit(param.meat_probe_tempMP1));    /*print, Measure MP1 probe temp in Farenheit*/

			param.data_buff=param.data[5+param.head_byte];
			param.data_buff=param.data_buff<<8;
			param.meat_probe_tempMP2=param.data[6+param.head_byte]|param.data_buff;       /*received MP2 probe*/
			//printf(" probe_MP2 = %d ",param.data_buff);                                 /*print MP2 probe ADC values*/
     
            printf("MP1 = %.2fC ", measure_Temp_Celsius(param.meat_probe_tempMP2));       /*print, Measure MP1 probe temp in Celsius*/
            printf("MP1 = %.2fF   ", measure_Temp_Fahrenheit(param.meat_probe_tempMP2));  /*print, Measure MP1 probe temp in Farenheit*/

			param.data_buff=param.data[9+param.head_byte];
			param.data_buff=param.data_buff<<8;
			param.meat_probe_tempMP3=param.data[10+param.head_byte]|param.data_buff;      /*received MP3 probe*/
			//printf(" probe_MP3 = %d \n",param.data_buff);
       
            printf("MP2 = %.2fC ", measure_Temp_Celsius(param.meat_probe_tempMP3));       /*print MP2 probe ADC values*/
            printf("MP2 = %.2fF  \n", measure_Temp_Fahrenheit(param.meat_probe_tempMP3)); /*print, Measure MP2 probe temp in Farenheit*/

			// vTaskDelay(10/portTICK_PERIOD_MS);    /*delay print*/
		}
		param.checkbyte=0;    /*clear byte checked*/ 
}
}

float measure_Temp_Celsius(int16_t probe_ADC){
	/*Calculate the resistance of the sensor*/ 
    float resistance = ((PT1000_MAX_RESISTANCE - PT1000_MIN_RESISTANCE)/
    ADC_MAX_VALUE*probe_ADC+PT1000_MIN_RESISTANCE)-RESISTANCE_GAIN;
	/*Calculate the temperature*/ 
	float temperature = (-A + sqrt(A * A - 4 * B * (1 - resistance / R0))) / (2 * B);
    return temperature;
}

float measure_Temp_Fahrenheit(int16_t probe_ADC){
	return (measure_Temp_Celsius(probe_ADC)*9)/5+32;
}

float measure_Pit_probe_celsius(){
	read_ADC_Venom();
    return measure_Temp_Celsius(param.meat_probe_tempMP1);	
}

float measure_Pit_probe_Fahrenheit(){
	read_ADC_Venom();
    return (measure_Temp_Celsius(param.meat_probe_tempMP1)*9)/5+32;	
}

void read_ADC_Venom(void){
param.rxBytes = uart_read_bytes(UART_NUM_1, param.data, RX_BUF_SIZE, 10/portTICK_PERIOD_MS);
if(param.rxBytes > 10) 	
{ 
	param.data[param.rxBytes] = 0;
	param.rxBytes=0;
	param.head_byte=0;
	for(param.rxi=0;param.rxi<50;param.rxi++)
		{
			if(param.data[param.rxi]==0xe1)
				{
					param.head_byte=param.rxi;
				    //printf("HeaD BYTE 1! %d\n",head_byte);
					break;
				}
		}
	for(param.rxi=param.head_byte;param.rxi<15+param.head_byte;param.rxi++)
		{
			param.checkbyte+=param.data[param.rxi];
			//printf("HeaD BYTE 2! %d\n",param.checkbyte);
		}	
	/*recieved earch adc value*/  
	if(param.data[param.head_byte]==0xe1&&param.checkbyte==param.data[15+param.head_byte])
		{
			param.data_buff=param.data[1+param.head_byte];
			param.data_buff=param.data_buff<<8;
			Knob_key_AD=param.data[2+param.head_byte]|param.data_buff;              /*received button press*/
			//printf("Buttons = %d  ",param.data_buff);
			param.data_buff=param.data[3+param.head_byte];
			param.data_buff=param.data_buff<<8;
			param.meat_probe_tempMP1=param.data[4+param.head_byte]|param.data_buff; /*received P probe*/
			//printf(" P = %d ",param.data_buff);                                  /*print P probe ADC values*/
			param.data_buff=param.data[5+param.head_byte];
			param.data_buff=param.data_buff<<8;
			param.meat_probe_tempMP2=param.data[6+param.head_byte]|param.data_buff; /*received MP2 probe*/
			//printf(" probe_MP2 = %d ",param.data_buff);                           /*print MP2 probe ADC values*/
			param.data_buff=param.data[9+param.head_byte];
			param.data_buff=param.data_buff<<8;
			param.meat_probe_tempMP3=param.data[10+param.head_byte]|param.data_buff;/*received MP3 probe*/
			//printf(" probe_MP3 = %d \n",param.data_buff);
		}
		param.checkbyte=0;    /*clear byte checked*/ 
}

}