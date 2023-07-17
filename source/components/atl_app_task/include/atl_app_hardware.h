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
 *     @Description:    The atl_app_hardware defines the hardware and Pinout 
 *    based on the application schematics specification, the pins can be change to
 *    fit any ATL controller for portability.
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#ifndef ATL_GLOBAL_DEFINE_H_
#define ATL_GLOBAL_DEFINE_H_
#include "driver/gpio.h"       /*include the ESP32 gpio*/

/*Uncomment the pinout define bellow to run for the Venom controller*/

/*MCPWM PIN Configuration*/
#define GPIO_PWM0A_OUT      GPIO_NUM_13   /*MPPWM Motor driver input pin*/ 

/*Encoder PIN Configuration*/
#define SW_A_PIN            GPIO_NUM_34
#define SW_B_PIN            GPIO_NUM_39

/*UART_Wundows PIN Configuration*/
#define UART_WIN_TXD       (GPIO_NUM_1)
#define UART_WIN_RXD       (GPIO_NUM_3)

/*UART_ADC PIN Configuration*/
#define UART_ARC_TXD       (GPIO_NUM_32) 
#define UART_ADC_RXD       (GPIO_NUM_35)  

/*Speaker PIN Configuration*/
#define SPEAKER_PIN         GPIO_NUM_23   /*Change the pin for the specific hardware*/

/*LEDC Motor PIN Configuration (Optional)*/
#define LEDC_HS_CH0_GPIO   (GPIO_NUM_13)   /*LEDC Motor driver input pin*/

/*max6675 PINS Configuration*/
#define CS_GPIO             GPIO_NUM_29
#define SCLK_GPIO           GPIO_NUM_29
#define MISO_GPIO           GPIO_NUM_29


/*uncomment the pinout defines bellow to run for KJ controller*/

// /*MCPWM PIN Configuration*/
// #define GPIO_PWM0A_OUT      GPIO_NUM_17                   /*MPPWM Motor driver input pin*/ 

// /*Encoder PIN Configuration*/
// #define SW_A_PIN            GPIO_NUM_18
// #define SW_B_PIN            GPIO_NUM_5

// /*UART_Wundows PIN Configuration*/
// #define UART_WIN_TXD       (GPIO_NUM_1)
// #define UART_WIN_RXD       (GPIO_NUM_3)

// /*UART_ADC PIN Configuration*/
// #define UART_ARC_TXD       (GPIO_NUM_32) 
// #define UART_ADC_RXD       (GPIO_NUM_35)  

// /*Speaker PIN Configuration*/
// #define SPEAKER_PIN         GPIO_NUM_16                   /*Change the pin for the specific hardware*/

// /*LEDC Motor PIN Configuration (Optional)*/
// #define LEDC_HS_CH0_GPIO   (GPIO_NUM_17)                  /*LEDC Motor driver input pin*/

// /*max6675 PINS Configuration*/
// #define CS_GPIO             GPIO_NUM_33
// #define SCLK_GPIO           GPIO_NUM_15
// #define MISO_GPIO           GPIO_NUM_13



#endif /* ATL_GLOBAL_DEFINE_H_ */
