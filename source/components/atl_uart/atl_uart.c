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
 *            @file:    atl_uart.c 
 *          @author:    Patrice Gama
 *            @date:    2022-02-21
 *       @Component:    atl_uart
 *              
 *     @Description:    The atl_uart protocol interfaces with the Windows 
 *     application to serve as a simulator for the controller algorithm 
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
#include "driver/uart.h"

#include "atl_app_hardware.h"
#include "atl_uart.h"

#define UART_WIN_RTS  (UART_PIN_NO_CHANGE)
#define UART_WIN_CTS  (UART_PIN_NO_CHANGE)

#define UART_ADC_RTS  (UART_PIN_NO_CHANGE)
#define UART_ADC_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)
static const int RX_BUF_SIZE = 1024;

char str[80];

struct IntegerHolder {
  uint8_t *data;
};

struct IntegerHolder datainit;

void atl_init_WIN_uart() {

    datainit.data=(uint8_t*) malloc(BUF_SIZE);
    /* Configure parameters for the UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    //uart_set_pin(UART_NUM_0, UART_WIN_TXD, UART_WIN_RXD, UART_WIN_RTS,UART_WIN_CTS);
    uart_set_pin(UART_NUM_0,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,UART_WIN_RTS,UART_WIN_CTS);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
}

char* read_win_uart() {
    int len0 = uart_read_bytes(UART_NUM_0, datainit.data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
    // uart0_bytes_len();
    // printf(" datalen %d \n",len);
    // printf(" datalen %d data uartbyte %d \n",uart0_bytes_len(),len);
	uart_write_bytes(UART_NUM_0,(const char*)datainit.data, uart0_bytes_len());
        if (len0) {
            datainit.data[len0] = '\0';
           // printf("my data %s \n",(char*)datainit.data);
            sprintf(str, "%s",(char*)datainit.data);
        } 
       // printf("data in %s \n", str);   
    return str;
}


void init_adc_uart(void)
{
    /*Configure parameters of an UART driver,
     *communication pins and install the driver*/

    const uart_config_t uart_config1 = {
        .baud_rate =9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    /*We won't use a buffer for sending data.*/ 
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE*2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1,&uart_config1);
    uart_set_pin(UART_NUM_1, UART_ARC_TXD, UART_ADC_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

/*function to hold the incomming and availablebyte legnth*/
int uart0_bytes_len(){ 
    int uart_len = uart_read_bytes(UART_NUM_0, datainit.data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
    return uart_len;
}