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
 *            @file:    atl_timer.h  
 *          @author:    Patrice Gama
 *            @date:    2022-02-21
 *       @Component:    atl_timer
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

#ifndef MAIN_TIMER_FUC_H_
#define MAIN_TIMER_FUC_H_

#define TIMER_BASE_CLK 12///new define for bug fix 
/*
 * A sample structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // 80m/16 convert counter value to seconds
#define TIMER_INTERVAL0_SEC   (0.01) // sample test interval for the first timer
#define TIMER_INTERVAL1_SEC   (5.78)   // sample test interval for the second timer
#define TEST_WITHOUT_RELOAD   0        // testing will be done without auto reload
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload


//void timer_pro(void);

//static void example_tg0_timer_init(int timer_idx,bool auto_reload, double timer_interval_sec);
extern uint8_t connect_long_count;
extern uint8_t t_500ms;

extern uint8_t t_1s;
extern uint8_t power_key;
extern uint8_t power_long_count;
extern uint8_t zuhe_long_count;

void timer0_init(void);
#endif /* MAIN_TIMER_FUC_H_ */
