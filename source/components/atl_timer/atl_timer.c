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
 *            @file:    atl_timer.c  
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

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "defineh.h"
#include "timer_fuc.h"

#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/ledc.h"

//extern xQueueHandle timer_queue;

uint8_t t_100ms=0;
uint8_t t_printf=0;
uint8_t t_500ms=0;

uint8_t t_2s=0;

uint8_t t_1s=0;
uint8_t power_long_count=0;
uint8_t power_key;
uint8_t connect_long_count=0;

uint8_t zuhe_long_count=0;
uint8_t t_10s;
__byte_type time_flag;
__byte_type time_flag1;
__byte_type time_flag2;
__byte_type time_flag3;

/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr(void *para)  //  �жϷ����� 100ms
{
    ///timer_event_t evt;
    int timer_idx = (int) para;

    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    // uint32_t intr_status = TIMERG0.int_st_timers.val;
    // TIMERG0.hw_timer[timer_idx].update = 1;
    // uint64_t timer_counter_value =
    //     ((uint64_t) TIMERG0.hw_timer[timer_idx].cnt_high) << 32
    //     | TIMERG0.hw_timer[timer_idx].cnt_low;

    /* Prepare basic event data
       that will be then sent back to the main program task */
    // timer_start_t evt;
    // evt.timer_group = 0;
    // evt.timer_idx = timer_idx;
    // evt.timer_counter_value = timer_counter_value;

    /* Clear the interrupt
       and update the alarm time for the timer with without reload */
    // if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0)
    // {
    //     evt.type = TEST_WITH_RELOAD;
    //     // TIMERG0.int_clr_timers.t0 = 1;
    //     //timer_counter_value += (uint64_t) (TIMER_INTERVAL0_SEC * TIMER_SCALE);
    //     //TIMERG0.hw_timer[timer_idx].alarm_high = (uint32_t) (timer_counter_value >> 32);
    //     //TIMERG0.hw_timer[timer_idx].alarm_low = (uint32_t) timer_counter_value;
    // }
    // else if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_1)
    // {
    //     evt.type = TEST_WITH_RELOAD;
    //     // TIMERG0.int_clr_timers.t1 = 1;
    // }
    // else
    // {
    //     evt.type = -1; // not supported even type
    // }

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    TIMERG0.hw_timer[timer_idx].config.tx_alarm_en = TIMER_ALARM_EN;
	f_10ms=1;
	if(++t_100ms>=10)
		{
			t_100ms=0;
   		 	f_100ms=1;
			f_100ms1=1;
			f_100ms2=1;
			f_100ms3=1;

			if(++t_500ms>=4)
				{
					t_500ms=0;
				}
			
			if(++t_printf>=50)
				{
					t_printf=0;
				}
			if(++t_1s>=10)
				{
					t_1s=0;
					f_1s=1;
				}
			
			if(++t_2s>=20)
				{
					t_2s=0;
					f_2s=1;
					if(beep_cnt!=0)
					{
						if(disp_statu==ACT_MEAT||scroll_mode==1)
							{
								beencnt=15;
							}
						else
							{
								beencnt=0;
							}
					}
				}
			
			if(b_power_on==1)
			{
				if(++t_10s>=psArgument_T)
				{
					t_10s=0;
					f_10s=1;
				}
			}
			
			if(power_key==1)
				{
					++power_long_count;
				}

			if(connect_keybat==1)
				{
					++connect_long_count;
				}

			if(b_zuhe_key==1)
				{
					++zuhe_long_count;
				}
			
			if(timer_key==1)
				{
					++timer_longcount;
				}
			 /* Now just send the event data back to the main program task */
   			 //xQueueSendFromISR(timer_queue, &evt, NULL);
		}
   
}

/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
static void example_tg0_timer_init(int timer_idx,
    bool auto_reload, double timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP; // ������ʱ
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;  //  ��������  TIMER_ALARM_DIS    TIMER_ALARM_EN
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = auto_reload; // �Զ����¼��أ�
    timer_init(TIMER_GROUP_0, timer_idx, &config); //��ʱ���� 0�� ��ʼ����ʱ��

    /* Timer's counter will initially start from value below.Ԥ������Ҫ������ֵ��
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    // Ϊ�ض���ʱ����Ͷ�ʱ��ע���жϴ�������
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
        (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);  //  ����
}

void timer0_init(void)
{
	example_tg0_timer_init(TIMER_0, TEST_WITH_RELOAD,  TIMER_INTR_LEVEL);
}
/*
 * In this example, we will test hardware timer0 and timer1 of timer group0.
 */
