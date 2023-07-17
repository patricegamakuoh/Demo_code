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
 *            @file:    atl_motor_control.h  
 *          @author:    Patrice Gama
 *            @date:    2023-12-4
 *       @Component:    atl_motor_control
 *              
 *     @Description:   
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

#ifndef MAIN_PWMC_H_
#define MAIN_PWMC_H_

/*initialize the LEDC driver (fan PWM)*/
void fan_pwmc_init(void);
/*initialize the MCPWM driver (fan PWM)*/
void atl_fan_init(void);
/*input duty cycle speed from 0% to 100%*/
uint16_t write_PWM(float PWM_dutyCycle);

#endif /* MAIN_PWMC_H_ */
