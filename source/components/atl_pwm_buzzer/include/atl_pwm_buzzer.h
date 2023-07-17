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
 *            @file:    atl_speaker.h    
 *          @author:    Patirce Gama
 *            @date:    2023-04-7
 *       @Component:    atl_speaker
 *              
 *     @Description:    The atl speaker provide's the beep sounds for the 
 *    controller, the speaker name is used with the intension that the beep sound 
 *    can later be change to real human like voise . 
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#ifndef MAIN_SPEAKER_H_
#define MAIN_SPEAKER_H_
/*init buzzer */
void atl_speaker_init(void);
/*call buzzer timer */
void buzzer_sound(int count, int duration_ms, int timer_interval);
void buzzer_timer_callback(void* arg, int val);

#endif /* MAIN_SPEAKER_H_ */
