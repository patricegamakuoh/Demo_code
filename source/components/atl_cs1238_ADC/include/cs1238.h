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

#ifndef MAIN_CS1238_H_
#define MAIN_CS1238_H_

extern uint8_t n_zero_front;
extern uint8_t CHECK_cs1238ok;
extern uint8_t R_Temp_byte;

extern uint32_t F_Read_AD(void);
extern uint8_t F_Rd_AdReg(void);
extern void F_Wr_AdReg(uint8_t R_AdReg);
extern uint32_t F_Rd_AdReg_data(void);

#endif /* MAIN_TM1629_H_ */
