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
 *            @file:    atl_encoder.h 
 *          @author:    Patrice Gama
 *            @date:    2022-02-21
 *       @Component:    atl_encoder_knob
 *              
 *     @Description:    The atl_encoder reads the rotary encoder value via the ISR
 *    service and add an ISR handler function to detect the A pin's rising and 
 *    falling edges and determine the direction of rotation based on the B pin's 
 *    level. Finally, the current encoder are counted every seconds.
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
#ifndef ATL_ENCODER_H_
#define ATL_ENCODER_H_

/*read the encoder raw values*/
int32_t readEncoder_count(void);
/*initialize the encoder*/
void atl_encoder_init(void);

#endif /* ATL_ENCODER_H_ */
