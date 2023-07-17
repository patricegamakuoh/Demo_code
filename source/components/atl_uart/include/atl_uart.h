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
 *            @file:    atl_uart.h 
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

#ifndef ATL_UART_H_
#define ATL_UART_H_

 /*initialize uart for the Windows serial communication*/
void atl_init_WIN_uart();
/*read incomming data bytes from Windows serial*/
char* read_win_uart();
/*Initialize uart for the XHS28 ADC*/
void init_adc_uart();
/*get uart0 byte legnth*/
int uart0_bytes_len();

#endif /* ATL_UART_H_ */
