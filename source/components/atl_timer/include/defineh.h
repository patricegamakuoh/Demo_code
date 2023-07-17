/*
 * defineh.h
 *
 *  Created on: 2018��11��22��
 *      Author: Administrator
 */

#ifndef MAIN_DEFINEH_H_
#define MAIN_DEFINEH_H_


#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

typedef uint8_t  u8;
typedef uint16_t  u16;

typedef unsigned char __1_byte;
typedef struct 
{
 		unsigned  b0:1;
 		unsigned  b1:1;
 		unsigned  b2:1;
 		unsigned  b3:1;
 		unsigned  b4:1;
 		unsigned  b5:1;
 		unsigned  b6:1;
 		unsigned  b7:1;
}__8_bits;

typedef union 
{
 	__8_bits bits;
 	__1_byte byte;
} __byte_type;

typedef union 
{
    u16 Val;
    u8  v[2];
    struct
    {
        u8 LB;
        u8 HB;
    } byte;
    struct
    {
         u8 b0:1;
         u8 b1:1;
         u8 b2:1;
         u8 b3:1;
         u8 b4:1;
         u8 b5:1;
         u8 b6:1;
         u8 b7:1;
         u8 b8:1;
         u8 b9:1;
         u8 b10:1;
         u8 b11:1;
         u8 b12:1;
         u8 b13:1;
         u8 b14:1;
         u8 b15:1;
    } bits;
} UINT16_VAL, UINT16_BITS;


#define GPIO_OUTPUT_DATA           23
#define GPIO_OUTPUT_WR             22
#define GPIO_OUTPUT_CS             21
#define GPIO_INPUT_KNOBPRESS       19   //  
#define GPIO_OUTPUT_FAN            17
#define GPIO_OUTPUT_BL_LED         13   //  

#define GPIO_INPUT_NTC_COM    		39
#define GPIO_INPUT_KEY_AD     		36
#define GPIO_INPUT_DoorSWItch    	25
#define GPIO_INPUT_BOXDOOR       	26

#define GPIO_INPUT_SW_A    	        18
#define GPIO_INPUT_SW_B    	        5

#define PROBE_DOUT_1   	            4
#define PROBE_SCLK_1   	            27

//KJ Controller pin define 
// #define PROBE_DOUT_1   	            14
// #define PROBE_SCLK_1   	            23

#define MEATPROBE_MP1   	        32
#define MEATPROBE_MP2   	        33
#define MEATPROBE_MP3   	        34
#define MEATPROBE_MP4   	        35

#define  version_id    			    3
#define  Rversion_id    			0
#define bluelen  	    	20

#define uart1_dete        	time_flag.bits.b0
#define R_smp_rst1       	time_flag.bits.b1
#define f_memory      		time_flag.bits.b2
#define f_over_165f          time_flag.bits.b3
#define close_blue_ok     		time_flag.bits.b4
#define f_5s6  			time_flag.bits.b5
#define f_5s7   		time_flag.bits.b6
#define f_5s8      		time_flag.bits.b7

#define f_5s15   		time_flag1.bits.b0
#define f_5s16     		time_flag1.bits.b1
#define f_5s9      		time_flag1.bits.b2
#define updata_baifenbat      time_flag1.bits.b3
#define b_temp_key     	time_flag1.bits.b4
#define connect_static  		time_flag1.bits.b5
#define updata_baifen   		time_flag1.bits.b6
#define connect_aperror      	time_flag1.bits.b7

#define f_100ms3        time_flag2.bits.b0
#define f_100ms2        time_flag2.bits.b1
#define f_100ms      	time_flag2.bits.b2
#define f_100ms1        time_flag2.bits.b3
#define f_2s     		time_flag2.bits.b4
#define f_1s  			time_flag2.bits.b5
#define ER3   			time_flag2.bits.b6
#define meat1ER5      	time_flag2.bits.b7

#define f_10ms        time_flag3.bits.b0
#define b_meat_key    time_flag3.bits.b1
#define b_zuhe_key     time_flag3.bits.b2
#define f_5s17      	time_flag3.bits.b3
#define f_10s      		time_flag3.bits.b4
#define meat2ER5      	time_flag3.bits.b5
#define meat3ER5      	time_flag3.bits.b6
#define meat4ER5      	time_flag3.bits.b7

#define ER2        error_byte.bits.b0
#define ER4    		error_byte.bits.b1
#define ER5		     error_byte.bits.b2
#define ER4bat      	error_byte.bits.b3
#define f_reach      		error_byte.bits.b4
#define st      			error_byte.bits.b5
#define b_get_data      	error_byte.bits.b6
#define check_self_ing      	error_byte.bits.b7

#define SET_GRILL  		0
#define SET_MEAT   		1
#define ACT_GRILL  		2
#define ACT_MEAT   		3
#define SET_TIME  	  	4
#define ACT_TIME  	    5

#define pwmc_initduty   2100
#define pwmc_maxduty    16384
#define pwmc_minduty    1100

extern void CodingsWitchPolling_J4(void);// 
extern void clear_setmeat(void);
extern void turn_off(void);
extern int myRound(float d);

extern __byte_type time_flag;
extern __byte_type time_flag1;
extern __byte_type time_flag2;
extern __byte_type time_flag3;
extern __byte_type error_byte;
extern wifi_ap_record_t wifidata;
extern uint8_t update_static;

extern unsigned char fff2ReadIndex;
extern uint16_t max_set_temp;
extern uint16_t min_set_temp;
extern uint8_t meat_probe_err[4];
extern int32_t act_meat_temp[4];
extern uint16_t set_temp_meat[4];
extern uint8_t btimer_pause;
extern char open_door;
extern uint16_t set_temp_meatbat[4];
extern unsigned char isDeviceConnectFlag;
extern uint8_t appwrite_value[20];
extern uint8_t rx_data_static;
extern uint8_t appwite_len;
extern uint8_t count_up_domwn;
extern uint8_t timer_hourUP;
extern uint8_t timer_minUP;
extern uint8_t connect_alrealy;
extern uint8_t b4_byte3bit1;
extern unsigned char device_busy;
extern uint8_t upgrde_init;
extern uint8_t exit_time;
extern uint8_t timer_alarm;
extern uint8_t timer_hourbat;
extern uint8_t timer_minbat;
extern uint8_t timer_secbat;
extern uint8_t disp_static;
extern uint8_t timer_alarmend;
extern char update_ver[8];
extern char update_verbat[8];
extern char update_verbat1[8];
extern unsigned char sendcount;
extern unsigned char wifi_control_endable;
extern uint8_t wifi_shown_contrl;
extern unsigned char wifi_control_endablebat;
extern uint8_t ounce_count;
extern uint8_t b_beenon;
extern uint8_t meat_probe_channl;
extern uint8_t f_first_over;
extern uint8_t f_first_over1;
extern uint8_t f_first_over2;
extern uint8_t f_first_over3;
extern uint8_t disp_static;
extern uint8_t disp_statu;
extern uint8_t connect_ap_time;
extern float disp_data_buffWBf;
extern int16_t disp_data_buffWB;
extern uint8_t meat_probe_errshort[4];

extern uint8_t disp_statu;
extern uint8_t scroll_mode;
extern char app_disconnect;
extern uint8_t timer_uplong;
extern uint16_t fan_per;
extern unsigned char isCOnnectAWSShadow;
extern unsigned char wifi_connect;
extern uint8_t showdow_callback;
extern unsigned char isCallbackFlag;
extern char isNeedSetOk;
extern uint8_t setmeat_probe_ok[4];

extern int16_t psArgument_Kp;
extern int16_t psArgument_T;
extern int16_t psArgument_Ti;
extern int16_t psArgument_Td;
extern uint16_t psArgument_pwmCycle;
extern int32_t psArgument_SEk;
extern uint8_t beep_cnt;
extern uint8_t Fan_step;
extern uint8_t Fan_step_2;

extern uint8_t timer_key;
extern uint8_t timer_longcount;
extern uint8_t connect_keybat;
extern unsigned char isErrorConnectAWS;
extern char xssid[32];
extern char xpwd[64];
extern char xssidbat[32];
extern char xpwdbat[64];
extern char blue_MAC1[6];
extern const uint8_t *blue_MAC;
extern char *targerbuf;
extern uint16_t timer_all;
extern uint8_t beencnt;
extern uint8_t set_temp_ok1;
extern uint8_t set_temp_ok;
extern uint16_t gtimer;

extern uint8_t grill_probe_err;
extern uint16_t set_temp_grillbat;
extern unsigned char upgradeStep;

extern uint8_t updateTrigger;
extern uint8_t close_blue;

extern uint8_t set_temp_ok;

extern int16_t Searing_Offset;
extern uint16_t out_end;
extern char b_power_on;
extern int16_t meat_temp_offset1;
extern int16_t meat_temp_offset2;
extern int16_t meat_temp_offset3;
extern int16_t meat_temp_offset4;
extern uint8_t f_c_f;
extern uint8_t connect_key;
extern int adjust_index;

extern uint16_t set_temp_grill;
extern uint16_t report_timemin;
extern int32_t act_grill_temp;
extern uint8_t timer_hour;
extern int8_t timer_min;
extern uint8_t timer_hourbat;
extern uint8_t timer_minbat;

extern uint8_t warn_count;
extern uint8_t beep_oneces;

#endif /* MAIN_DEFINEH_H_ */
