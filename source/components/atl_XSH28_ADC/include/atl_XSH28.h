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
 *            @file:    cs1238.h   
 *          @author:    third party administrator
 *            @date:    2018
 *       @Component:    atl_cs1238_ADC
 *              
 *     @Description:    The cs1238 is an ADC converter that reads temperature from
 *    the meat probes, this is the header file that contents all the fuctions from
 *    lib.a archive file
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#ifndef MAIN_XSH28_H_
#define MAIN_XSH28_H_

/* Receive sensor data and button value from XSH28 ADC */
void receive_XSH28_ADC();

/* Initialize the XSH28 ADC */
void atl_XSH28_ADC_init();

/* measure the Pit probe temperature in C */
float measure_Pit_probe_celsius();

/* measure the Pit probe temperature in F */
float measure_Pit_probe_Fahrenheit();

#endif /* MAIN_XSH28_H_ */
