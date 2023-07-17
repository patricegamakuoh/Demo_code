/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
/**
 * @file thing_shadow_sample.c
 * @brief A simple connected window example demonstrating the use of Thing Shadow
 *
 * See example README for more details.
 */

#include "aws_iot.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
//#include "esp_event_loop.h"
#include "esp_event.h"

#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"

#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_jobs_interface.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "cJSON.h"
#include "defineh.h"
#include "wifi_ble_handle.h"

#include "esp32_upgrade.h"
#include "factory_uart.h"

#define UpgradeVersion			5

// 是否订阅了需要的回调
unsigned char isCallbackFlag = 0;
unsigned char Callback_count = 0;

unsigned char awsUpgradeStep = 0;
unsigned int waitUpgradeTimeCount = 0;
unsigned int delayCount = 0;
float progressData = 0;


#define awsUpgradeStepCheckTaskList				0
#define awsUpgradeStepWaitListResponse			1
#define awsUpgradeStepCycleTask					2
#define awsUpgradeStepWaitTaskResponse			3
#define awsUpgradeStepUpgradeNow				4
#define awsUpgradeStepQuit						5

extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_private_pem_key_end");

char checkUpgrade(AWS_IoT_Client *client);

static const char *TAG = "AWS";
const int UPGRADE_CONNECTED_BIT = BIT0;

#define CLIENT_ID 		"esp32mbABCDEF"
//#define ThingName		"4fe8c4404a2819f0b6026f12f128e41f"

uint8_t updateTrigger=0;
uint8_t update_static=0;
char update_ver[8]={'3','.','0','.','0','1'};
char update_verbat[8];
char update_verbat1[8];

extern char rootca[CaCerKeyMaxLen];
extern char cer[CaCerKeyMaxLen];
extern char privatekey[CaCerKeyMaxLen];
extern char	ThingName[ThingNameUartMaxLen];
extern unsigned char BlueMacString[12];
extern unsigned char BlueMacStringTemp[18];

static EventGroupHandle_t upgrade_event_group;
char HostAddress[255] = "a386xm06thrxxr-ats.iot.us-east-2.amazonaws.com";
//uint32_t port = 443;
uint32_t port = 8883;
unsigned char sendcount=0;
char b_desired;
uint16_t set_temp_grillbat;
uint16_t timer_allbat;
unsigned short sub_time=100;

char b_desiredinit=0;
char b_desiredinitbat=0;

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) 
{
    ESP_LOGI(TAG, "MQTT Disconnect..");
    IoT_Error_t rc = FAILURE;

    if(NULL == pClient)
	{
        return;
    }

    if(aws_iot_is_autoreconnect_enabled(pClient)) 
	{
        ESP_LOGI(TAG, "Auto Reconnect enabled, Reconnecting attempt will start now..");
    }
	else 
	{
        ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if(NETWORK_RECONNECTED == rc) 
		{
			//isErrorConnectAWS=0;
            ESP_LOGW(TAG, "Manual Reconnect Successful");
        } 
		else 
		{
			//isErrorConnectAWS=1;
            ESP_LOGW(TAG, "Manual Reconnect Failed - %d", rc);
        }
    }
}

char sendRequestUpdateStatus(AWS_IoT_Client *pClient,char *msg,unsigned int len);
char sendRequestGet(AWS_IoT_Client *pClient);
char subscribeGetReject(AWS_IoT_Client *pClient);
char subscribeGetAccept(AWS_IoT_Client *pClient);
char subscribeUpdateDocument(AWS_IoT_Client *pClient);
char subscribeUpdateReject(AWS_IoT_Client *pClient);
char subscribeUpdateAccept(AWS_IoT_Client *pClient);
char controlMethon(AWS_IoT_Client *pClient);

/***********************************************************************/

char subscribeRequestTaskList(AWS_IoT_Client *pClient);
char sendRequestTaskList(AWS_IoT_Client *pClient);
char sendJobSucceed(AWS_IoT_Client *pClient,char *jobid);
char sendRequestJobDetails(AWS_IoT_Client *pClient,char *jobid);


void iot_request_tasklisk_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData);
void iot_request_jobdetails_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData);



IoT_Client_Connect_Params connectParams;


AWS_IoT_Client client;
void aws_iot_task(void *param) 
{
	
	IoT_Error_t rc = FAILURE;

	//初始化 aws
	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
	
	
	mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = HostAddress;
    mqttInitParams.port = port;
	
	mqttInitParams.pRootCALocation = (const char *)rootca;//aws_root_ca_pem_start;
    mqttInitParams.pDeviceCertLocation = (const char *)cer;//certificate_pem_crt_start;
    mqttInitParams.pDevicePrivateKeyLocation = (const char *)privatekey;//private_pem_key_start;
	
	mqttInitParams.mqttCommandTimeout_ms = 40000;
    mqttInitParams.tlsHandshakeTimeout_ms = 8000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;
	
	rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(SUCCESS != rc) 
	{
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", rc);
    }
	
    xEventGroupWaitBits(upgrade_event_group, UPGRADE_CONNECTED_BIT,
                        false, true, portMAX_DELAY);

	ESP_LOGI(TAG,"start aws");
	vTaskDelay(1000 / portTICK_RATE_MS);
    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;

    //connectParams.pClientID = CLIENT_ID;
    //connectParams.clientIDLen = (uint16_t) strlen(CLIENT_ID);

	connectParams.pClientID = ThingName;
    connectParams.clientIDLen = (uint16_t) strlen(ThingName);
    connectParams.isWillMsgPresent = false;
	
    ESP_LOGI(TAG, "Connecting to AWS...");
    do 
	{
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if(SUCCESS != rc)
		{
			isErrorConnectAWS=1;  //Can’t connect to AWS    -0x6800  /**< The operation timed out. */
           		 ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
            		vTaskDelay(1000 / portTICK_RATE_MS);
        	}
		else
		{
			isErrorConnectAWS=0;
			ESP_LOGI(TAG, "Connected to AWS ok");
		}
    } while(SUCCESS != rc);

    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if(SUCCESS != rc) 
	{
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc);
    }
	isConnectAWS=1;
	
	while(1)
	{
		
		// 控制循环
		controlMethon(&client);
		// 升级检查
		checkUpgrade(&client);
		vTaskDelay(100 / portTICK_RATE_MS);
		//  // 检查是否需要升级版本
		rc = aws_iot_mqtt_yield(&client, 100);
	}
    vTaskDelete(NULL);
	
}
//////////////////////////////////////////
void initAWSIot(void)
{
	upgrade_event_group = xEventGroupCreate();
    xTaskCreatePinnedToCore(&aws_iot_task, "aws_iot_task", 8192, NULL, 3, NULL, 1);
}

void setAwsWiFiConnectd(void)
{
    xEventGroupSetBits(upgrade_event_group, UPGRADE_CONNECTED_BIT);
}

void resetAwsWiFiConnectd(void)
{
    xEventGroupClearBits(upgrade_event_group, UPGRADE_CONNECTED_BIT);
}

int maxUpgradeVersion = 0;
char upgradeJobIdBuf[34];
void dealJobID(char *str,AWS_IoT_Client *pClient)
{
	if(str!=NULL&&strlen(str)>=8)
	{
		char ver[34];
		memset(ver,0,34);
		memcpy(ver,&str[7],(strlen(str)-7));
		ESP_LOGI(TAG,"Ver:%s",ver);
		//UpgradeVersion
		int iver = atoi(ver);
		if(iver<=UpgradeVersion)
		{
			// 上报已经成功了
			ESP_LOGI(TAG,"have upgraded:%s",str);
			sendJobSucceed(pClient,str);

			upgrde_init=1;
		}
		else
		{
			// 进行下载
			ESP_LOGI(TAG,"need upgrade:%s",str); // AWS: need upgrade:OTA_MB_014
			if(iver>=maxUpgradeVersion)
			{
				maxUpgradeVersion = iver;
				memset(upgradeJobIdBuf,0,34);
				strcpy(upgradeJobIdBuf,str);
			}
		}
	}
}

//Request callback, $aws/things/masterbuilt-esp/jobs/get/accepted
//{"timestamp":1562310286,"inProgressJobs":[],"queuedJobs":[]}
// 订阅任务队列回调
void iot_request_tasklisk_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData) 
{
	
    ESP_LOGI(TAG, "Request callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);
	maxUpgradeVersion = 0;
	//cJSON_Delete
	cJSON * root = cJSON_Parse((char *)params->payload);
	if(root!=NULL)
	{
		cJSON * jobsItem = cJSON_GetObjectItem(root, "queuedJobs");
		if(jobsItem!=NULL)
		{
			int size = cJSON_GetArraySize(jobsItem);
			if(size>0)
			{
				for(int i=0;i<size;i++)
				{
					cJSON *job = cJSON_GetArrayItem(jobsItem,i);
					if(job!=NULL)
					{
						cJSON *jobid = cJSON_GetObjectItem(job, "jobId");
						if(jobid!=NULL)
						{
							char *strJobID = jobid->valuestring;
							ESP_LOGI(TAG,"JobID:%s  --%d",strJobID,i);
							dealJobID(strJobID,pClient);
							
						}
						else
						{
							ESP_LOGI(TAG,"JobID null");
							upgrde_init=1;
						}
					}
					else
					{
						ESP_LOGI(TAG,"job empty");
						upgrde_init=1;
					}
				}
			}
			else
			{
				upgradeStep=0;
				close_blue=0;
				//updateTrigger=0;
				upgrde_init=1;
				ESP_LOGI(TAG,"queue jobs zero");
			}
		}
		else
		{
			ESP_LOGI(TAG,"no queue jobs");
			//updateTrigger=0;
			upgradeStep=0;
			upgrde_init=1;
		}
		cJSON_Delete(root);
	}
}

/*
AWS: Request job details callback, $aws/things/masterbuilt-esp/jobs/OTA_MB_045
/get/accepted  {"timestamp":1562290096,"execution":{"jobId":"OTA_MB_045","
status":"QUEUED","queuedAt":1562290028,"lastUpdatedAt":1562290028,"
versionNumber":1,"executionNumber":1,"jobDocument":{"ver":"V0.2.4","url":"
https://esp32upgrade.s3.us-east-2.amazonaws.com/OTA_MB_024.bin","len":"1267552"}}}
I (114280) AWS: url:https://esp32upgrade.s3.us-east-2.amazonaws.com/OTA_MB_024.bin
*/
// 任务详情 回调
unsigned int upgradeFileLen = 0;
char upgradeUrl[128];
void iot_request_jobdetails_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
		IoT_Publish_Message_Params *params, void *pData) 

{
    ESP_LOGI(TAG, "Request job details callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);

	cJSON * root = cJSON_Parse((char *)params->payload);
	if(root!=NULL)
	{
		cJSON * executionItem = cJSON_GetObjectItem(root, "execution");
		if(executionItem!=NULL)
		{
			cJSON * jobDocumentItem = cJSON_GetObjectItem(executionItem,"jobDocument");
			if(jobDocumentItem!=NULL)
			{
				cJSON * verItem = cJSON_GetObjectItem(jobDocumentItem,"ver");
				cJSON * urlItem = cJSON_GetObjectItem(jobDocumentItem,"url");
				cJSON * lenItem = cJSON_GetObjectItem(jobDocumentItem,"len");

				if(urlItem!=NULL)
				{
					memset(upgradeUrl,0,128);
					strcpy(upgradeUrl,urlItem->valuestring);
					ESP_LOGI(TAG,"url:%s",upgradeUrl);
					
					upgradeFileLen = atoi(lenItem->valuestring);
					if(close_blue_ok==0)
						{
							if(strcmp(verItem->valuestring,update_ver)>0 && check_self_ing==0)
							{
								memset(update_verbat1,0,8);
								upgradeStep=2;
								strcpy(update_verbat1,verItem->valuestring);
								ESP_LOGI(TAG,"ver:%s",update_verbat1);
							}
							else
							{
								upgradeStep=0;
								upgrde_init=1;
								awsUpgradeStep =awsUpgradeStepQuit;
								//ESP_LOGI(TAG,"awsUpgradeStep:%d",awsUpgradeStep);
								ESP_LOGI(TAG, "old Version");
							}	
						}
					else
						{
							memset(update_verbat1,0,8);
							strcpy(update_verbat1,verItem->valuestring);
							ESP_LOGI(TAG,"ver2:%s",update_verbat1);
						}
					
				}
				else
				{
					ESP_LOGI(TAG,"url error");
				}
				
			}
			else
			{
				ESP_LOGI(TAG,"jobDocument error");
			}
		}
		else
		{
			ESP_LOGI(TAG,"executionItem error");
		}
		cJSON_Delete(root);
	}
}

char topicSusbicPending[100];
char subscribeRequestTaskList(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicPending,0,100);
	sprintf(topicSusbicPending,"$aws/things/%s/jobs/get/+",ThingName);
	topicLen = strlen(topicSusbicPending);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicPending);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicPending, topicLen, QOS0, iot_request_tasklisk_callback_handler, NULL);
    if(SUCCESS != rc) 
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error5 subscribing %s: %d ",topicSusbicPending,rc);
        return 0;
    }
    else
    {
	sub_time=100;
	Callback_count=1;
    }
    return 1;
}

// 订阅任务详情
char topicSusbicJobsDetails[100];
char subscribeRequestJobDetails(AWS_IoT_Client *pClient,char *jobid)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicJobsDetails,0,100);
	sprintf(topicSusbicJobsDetails,"$aws/things/%s/jobs/%s/get/accepted",ThingName,jobid);
	topicLen = strlen(topicSusbicJobsDetails);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicJobsDetails);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicJobsDetails, topicLen, QOS0, iot_request_jobdetails_callback_handler, NULL);
    if(SUCCESS != rc) 
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error6 subscribing %s: %d ",topicSusbicJobsDetails,rc);
        return 0;
    }
	
	return 1;
}

char topicToPublishJobSucceed[100];
char topicJobSucceedMsg[64];
char sendJobSucceed(AWS_IoT_Client *pClient,char *jobid)
{
	memset(topicToPublishJobSucceed,0,100);
	memset(topicJobSucceedMsg,0,64);
	//IoT_Error_t rc = FAILURE;
	
	sprintf(topicToPublishJobSucceed,"$aws/things/%s/jobs/%s/update",ThingName,jobid);
	sprintf(topicJobSucceedMsg,"{\"status\":\"SUCCEEDED\"}");
	
	IoT_Publish_Message_Params publishParams;
	publishParams.qos = QOS0;
	publishParams.isRetained = 0;
	publishParams.isDup = 0;
	publishParams.id = 0;
	publishParams.payload = topicJobSucceedMsg;
	publishParams.payloadLen = strlen(topicJobSucceedMsg);

	return aws_iot_mqtt_publish(pClient, topicToPublishJobSucceed, strlen(topicToPublishJobSucceed), &publishParams);	
		
}

char isNeedSetOk = 0;
void tellServerOTAOk(void)
{
	if(strlen(upgradeJobIdBuf)>0)
	{
		isNeedSetOk = 1;
		ESP_LOGI(TAG,"tell server ota succeed :%s",upgradeJobIdBuf);
	}
	
}
//$aws/things/4916a54866e2bf3d94dfca3b3d214f17/jobs/get
char topicToPublishGetPending[100];
char sendRequestTaskList(AWS_IoT_Client *pClient)
{
	memset(topicToPublishGetPending,0,100);
	IoT_Error_t rc = FAILURE;
	rc = aws_iot_jobs_send_query(pClient, QOS0, ThingName, NULL, NULL, topicToPublishGetPending, sizeof(topicToPublishGetPending), NULL, 0, JOB_GET_PENDING_TOPIC);
	ESP_LOGI(TAG,"Query:%s",topicToPublishGetPending);
	if(SUCCESS != rc) 
	{
        ESP_LOGI(TAG, "Error query topic %d",rc);
        return 0;
    }
	return 1;
}
char topicToPublishGetJobDetails[100];
char topicJobDetailsMsg[64];
char sendRequestJobDetails(AWS_IoT_Client *pClient,char *jobid)
{
	memset(topicToPublishGetJobDetails,0,100);
	sprintf(topicToPublishGetJobDetails,"$aws/things/%s/jobs/%s/get",ThingName,jobid);
	memset(topicJobDetailsMsg,0,64);
	sprintf(topicJobDetailsMsg,"{\"executionNumber\":1,\"includeJobDocument\":true}");
	
	IoT_Publish_Message_Params publishParams;
	publishParams.qos = QOS0;
	publishParams.isRetained = 0;
	publishParams.isDup = 0;
	publishParams.id = 0;
	publishParams.payload = topicJobDetailsMsg;
	publishParams.payloadLen = strlen(topicJobDetailsMsg);

	return aws_iot_mqtt_publish(pClient, topicToPublishGetJobDetails, strlen(topicToPublishGetJobDetails), &publishParams);	
	
}

// 0 开始查询是否有任务升级任务	1 等待任务升级任务的回复	 
// 2 有需要查询的任务，进行任务查询  3 等待任务队列查询   4 升级任务需要持行 
// 5不需要持行升级


char checkUpgrade(AWS_IoT_Client *pClient)
{
	if(isCallbackFlag==0&&wifi_connect==1&&++sub_time>=100)
	{
		device_busy=1;
		sub_time=0;
		switch(Callback_count)
		{
			case 0:
				subscribeRequestTaskList(pClient);

			break;

			case 1:
				subscribeGetReject(pClient);
			break;

			case 2: 
				subscribeGetAccept(pClient);
			break;

			case 3:
				subscribeUpdateDocument(pClient);
			break;

			case 4:
				subscribeUpdateReject(pClient);
			break;

			case 5:
				if(upgradeStepNow()!=3)
				{
					subscribeUpdateAccept(pClient);
				}
			break;

			case 6:
				
				isCallbackFlag = 1;
				Callback_count=0;
				isCOnnectAWSShadow=1;
			break;

		}
					
	}
	
	if(isNeedSetOk!=0&&wifi_connect==1)
	{
		isNeedSetOk = 0;
		sendJobSucceed(&client,upgradeJobIdBuf);	
	}
	
	if(updateTrigger==1)
		{
			//subscribeRequestTaskList(pClient);
		}
	
	if(updateTrigger!=0||upgrde_init==0)
		{
			delayCount++;
		}
	else
		{
			delayCount=0;
		}
	
	//ESP_LOGI(TAG,"awsUpgradeStep_2:%d",awsUpgradeStep);

	if(delayCount>10)
	{
		ESP_LOGI(TAG,"awsUpgradeStep_3:%d",awsUpgradeStep);
		delayCount = 0;
		switch(awsUpgradeStep)
		{
			case awsUpgradeStepCheckTaskList:
				if(sendRequestTaskList(pClient)!=0)
				{
					awsUpgradeStep = awsUpgradeStepWaitListResponse;
					waitUpgradeTimeCount = 0;
					return awsUpgradeStep;
				}
				else
				{
					awsUpgradeStep = awsUpgradeStepQuit;
					upgradeStep=0;
				}
				break;
			case awsUpgradeStepWaitListResponse:
				waitUpgradeTimeCount++;
				if(waitUpgradeTimeCount>10)
				{
					waitUpgradeTimeCount = 0;
					// 么有收到回复，则提示不需要升级了
					ESP_LOGI(TAG,"maxUpgradeVersion...:%d\n",maxUpgradeVersion);
					awsUpgradeStep = awsUpgradeStepQuit;
				}
				
				if(maxUpgradeVersion!=0)
				{
					if(maxUpgradeVersion>UpgradeVersion) // ESP_LOGI(TAG,"ver:%s",update_ver);
					{
						// $aws/things/masterbuilt-esp/jobs/OTA_MB_014/get/accepted
						subscribeRequestJobDetails(pClient,upgradeJobIdBuf);
						ESP_LOGI(TAG,"now to next upgrade step:%s",upgradeJobIdBuf);
						if(awsUpgradeStep !=awsUpgradeStepQuit)
						{
							awsUpgradeStep = awsUpgradeStepCycleTask;
						}
					}
				}
				
				break;
			case awsUpgradeStepCycleTask:
				// 获取jobid的信息
				memset(upgradeUrl,0,128);
				if(sendRequestJobDetails(pClient,upgradeJobIdBuf)!=0)
				{

				}
				awsUpgradeStep = awsUpgradeStepWaitTaskResponse;
				waitUpgradeTimeCount = 0;
				return awsUpgradeStep;
				//else{
				//	ESP_LOGI(TAG,"sendRequestJobDetails send error");
				//	awsUpgradeStep = awsUpgradeStepQuit;
				//}
				break;
			case awsUpgradeStepWaitTaskResponse:
				waitUpgradeTimeCount++;
				if(waitUpgradeTimeCount>10)
				{
					waitUpgradeTimeCount = 0;
					// 没有收到回复，则提示不需要升级了
					awsUpgradeStep = awsUpgradeStepQuit;
					ESP_LOGI(TAG,"awsUpgradeStepQuit %d:",waitUpgradeTimeCount);
				}
				
				// 如果的到了详细的内容，然后持行下一步升级
				if(strlen(upgradeUrl)>0)
				{                      //https://esp32upgrade.s3.us-east-2.amazonaws.com/OTA_MB_024.bin
					ESP_LOGI(TAG,"get upgrade url:%s  ",upgradeUrl);
					awsUpgradeStep = awsUpgradeStepUpgradeNow;
				}
				break;
			case awsUpgradeStepUpgradeNow:
				// 启动升级 并判断升级状态
				ESP_LOGI(TAG,"set url and filelen to ota thread :%s  %d",upgradeUrl,upgradeFileLen);
				setUrlUpgrade(upgradeUrl,upgradeFileLen);
				if(upgrde_init==0)
				{
					upgradeStep=3;
				}
				
				awsUpgradeStep=awsUpgradeStepQuit;
				break;
			case awsUpgradeStepQuit:
				//
				
				break;
			default:
				break;
		}
		//ESP_LOGI(TAG, "P:%f  Step:%d Count:%d Stack remaining for task '%s' is %d bytes",progressData,awsUpgradeStep,waitUpgradeTimeCount, pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));

	}
	return awsUpgradeStep;
}
void dealAwsData(char *msg,unsigned int len)
{
	if(len>0&&msg!=NULL)
	 {
		cJSON *root = cJSON_Parse(msg);
		if(root!=NULL)
		 {
			if(cJSON_HasObjectItem(root,"state"))
			{
				cJSON *state = cJSON_GetObjectItem(root,"state");
				if(state!=NULL)
				{
					//if(cJSON_HasObjectItem(state,"desired"))
					{
						
						//cJSON *desired = cJSON_GetObjectItem(state,"desired");
						//if(desired!=0)
						{
							
							//ESP_LOGI(TAG,"xxdesiredxx");
							if(cJSON_HasObjectItem(state,"model"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"model");
								ESP_LOGI(TAG,"Recv model:%s",dataType->valuestring);
								b_desired=1;
								ESP_LOGI(TAG,"b_desired:%d",b_desired);
							}
							
							if(cJSON_HasObjectItem(state,"vers"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"vers");
								ESP_LOGI(TAG,"Recv vers:%s",dataType->valuestring);
								b_desired=1;
								ESP_LOGI(TAG,"b_desired:%d",b_desired);
							}

							if(cJSON_HasObjectItem(state,"update"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"update");
								//update_static=dataType->valueint;
								b_desired=1;
								ESP_LOGI(TAG,"Recv update:%d",dataType->valueint);
							}

							if(cJSON_HasObjectItem(state,"trigger"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"trigger");
								updateTrigger=dataType->valueint;
								if(updateTrigger==1)
								{
									upgradeStep=1; //  	Checking for updates"
									awsUpgradeStep = awsUpgradeStepCheckTaskList;
								}
								b_desired=1;
								ESP_LOGI(TAG,"Recv trigger:%d",dataType->valueint);
							}

							if(cJSON_HasObjectItem(state,"mac"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"mac");
								b_desired=1;
								ESP_LOGI(TAG,"Recv mac:%s",dataType->valuestring);
							}
							
							if(cJSON_HasObjectItem(state,"pwrOn"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"pwrOn");
								b_power_on=dataType->valueint;
								beencnt=10;
								b_desired=1;
								if(b_power_on==0)
								{
									turn_off();
								}
								ESP_LOGI(TAG,"Recv isDeviceOn:%d",dataType->valueint);
							}
							
							if(cJSON_HasObjectItem(state,"fah"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"fah");
								if(dataType->valueint==0)
									{
										f_c_f=1;   //C
									}
								else
									{
										f_c_f=0;
									
}
									b_desired=1;
									beencnt=10;

							}
							
							if(cJSON_HasObjectItem(state,"engaged"))
							{
								cJSON *dataType = cJSON_GetObjectItem(state,"engaged");
								//b4_byte3bit1=dataType->valueint;
								b_desired=1;
								ESP_LOGI(TAG,"Recv engaged:%d",dataType->valueint);
							}
							
							if(cJSON_HasObjectItem(state,"doorOpn"))
							{
								//cJSON *dataType = cJSON_GetObjectItem(desired,"doorOpn");
								//open_door=dataType->valueint;
								//ESP_LOGI(TAG,"Recv doorOpn:%d",dataType->valueint);
								b_desired=1;
							}
							
							if(cJSON_HasObjectItem(state,"mainTemp"))
							{
								//cJSON *dataType = cJSON_GetObjectItem(desired,"mainTemp");
								//act_grill_temp=dataType->valuedouble;
								//ESP_LOGI(TAG,"Recv mainTemp:%f",dataType->valuedouble);
								b_desired=1;
							}

							if(cJSON_HasObjectItem(state,"heat"))
							{
								cJSON *heat = cJSON_GetObjectItem(state,"heat");
								if(heat!=NULL)
									{
										if(cJSON_HasObjectItem(heat,"t1"))
										{
											cJSON *type1 = cJSON_GetObjectItem(heat,"t1");
											if(type1!=NULL)
												{
													if(cJSON_HasObjectItem(type1,"heating"))
														{
															cJSON *dataType = cJSON_GetObjectItem(type1,"heating");
															b_desired=1;
															ESP_LOGI(TAG,"Recv heating:%d",dataType->valueint);
														}
													
													if(cJSON_HasObjectItem(type1,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(type1,"trgt");
															//set_temp_grill=dataType->valuedouble;
															//disp_statu=SET_GRILL;
															//adjust_index=5;
															//set_temp_ok1=1;
 															//f_over_165f=0;
 															//set_temp_ok=1;
															//gtimer=0;
															//exit_time=0;
															b_desired=1;
															ESP_LOGI(TAG,"Recv trgt:%d",dataType->valueint);
														}

													if(cJSON_HasObjectItem(type1,"intensity"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(type1,"intensity");
															//act_grill_temp=dataType->valuedouble;
															//ESP_LOGI(TAG,"Recv intensity:%f",dataType->valuedouble);
															b_desired=1;
														}

													if(cJSON_HasObjectItem(type1,"max"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(type1,"max");
															//max_set_temp=dataType->valueint;
															//ESP_LOGI(TAG,"Recv max:%d",dataType->valueint);
															b_desired=1;
														}

													if(cJSON_HasObjectItem(type1,"min"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(type1,"min");
															//min_set_temp=dataType->valueint;
															//ESP_LOGI(TAG,"Recv min:%d",dataType->valueint);
															b_desired=1;
														}
												}
										}
										
										if(cJSON_HasObjectItem(heat,"t2"))
										{
											cJSON *type2 = cJSON_GetObjectItem(heat,"t2");
											if(type2!=NULL)
												{
													if(cJSON_HasObjectItem(type2,"heating"))
														{
															cJSON *dataType = cJSON_GetObjectItem(type2,"heating");
															b_desired=1;
															ESP_LOGI(TAG,"Recv heating2:%d",dataType->valueint);
														}
													
													if(cJSON_HasObjectItem(type2,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(type2,"trgt");
															disp_data_buffWB=dataType->valueint;

															if(f_c_f==0) //F
															{
																set_temp_grill=disp_data_buffWB;
															}
															else
															{
																disp_data_buffWBf=disp_data_buffWB;
																disp_data_buffWBf=((disp_data_buffWBf*9)/5+32);
																set_temp_grill=myRound(disp_data_buffWBf);
															}	
															b_desired=1;
															ESP_LOGI(TAG,"Recv trgt:%d",dataType->valueint);

															beencnt=10;
															disp_statu=SET_GRILL;
															adjust_index=5;
															exit_time=0;
															set_temp_ok1=1;
 															f_over_165f=0;
 															set_temp_ok=1;
															gtimer=0;
														}

													if(cJSON_HasObjectItem(type2,"intensity"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(type2,"intensity");
															//act_grill_temp=dataType->valuedouble;
															//ESP_LOGI(TAG,"Recv intensity:%f",dataType->valuedouble);
															b_desired=1;
														}

													if(cJSON_HasObjectItem(type2,"max"))
														{
															cJSON *dataType = cJSON_GetObjectItem(type2,"max");
															max_set_temp=dataType->valueint;
															//ESP_LOGI(TAG,"Recv max:%d",dataType->valueint);
															b_desired=1;
														}

													if(cJSON_HasObjectItem(type2,"min"))
														{
															cJSON *dataType = cJSON_GetObjectItem(type2,"min");
															min_set_temp=dataType->valueint;
															b_desired=1;
															//ESP_LOGI(TAG,"Recv min:%d",dataType->valueint);
														}
												}
										}

										
									}
							}


						if(cJSON_HasObjectItem(state,"probes"))
							{
								cJSON *meatProbes = cJSON_GetObjectItem(state,"probes");
								if(meatProbes!=NULL)
									{
										if(cJSON_HasObjectItem(meatProbes,"p1"))
										{
											cJSON *probe1 = cJSON_GetObjectItem(meatProbes,"p1");
											if(probe1!=NULL)
												{
													if(cJSON_HasObjectItem(probe1,"temp"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(probe1,"temp");
															//act_meat_temp[0]=dataType->valuedouble;
															//ESP_LOGI(TAG,"Recv temp:%f",dataType->valuedouble);
															b_desired=1;
														}
													
													if(cJSON_HasObjectItem(probe1,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(probe1,"trgt");
															b_desired=1;
															if(meat_probe_err[0]==0)
																{
																	disp_data_buffWB=dataType->valueint;
																	if(f_c_f==0) //F
																	{
																		set_temp_meat[0]=disp_data_buffWB;
																	}
																	else
																	{
																		disp_data_buffWBf=disp_data_buffWB;
																		disp_data_buffWBf=((disp_data_buffWBf*9)/5+32);
																		set_temp_meat[0]=myRound(disp_data_buffWBf);
																	}	
																	disp_static=3;
																	exit_time=0;
																	disp_statu=SET_MEAT;    // 设定肉探针温度
																	adjust_index=6;
																	f_first_over=0;
																	beencnt=10;
																	clear_setmeat();
																	meat_probe_channl=0;	
																	setmeat_probe_ok[0]=1;
																}
															ESP_LOGI(TAG,"Recv trgt:%d",dataType->valueint);
														}
												}
										}
										
										if(cJSON_HasObjectItem(meatProbes,"p2"))
										{
											cJSON *probe2 = cJSON_GetObjectItem(meatProbes,"p2");
											if(probe2!=NULL)
												{
													if(cJSON_HasObjectItem(probe2,"temp"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(probe2,"temp");
															//act_meat_temp[1]=dataType->valuedouble;
															//ESP_LOGI(TAG,"Recv temp:%f",dataType->valuedouble);
															b_desired=1;
														}
													
													if(cJSON_HasObjectItem(probe2,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(probe2,"trgt");
															b_desired=1;
															if(meat_probe_err[1]==0)
																{
																	disp_data_buffWB=dataType->valueint;
																	if(f_c_f==0) //F
																	{
																		set_temp_meat[1]=disp_data_buffWB;
																	}
																	else
																	{
																		disp_data_buffWBf=disp_data_buffWB;
																		disp_data_buffWBf=(disp_data_buffWBf*9)/5+32;
																		set_temp_meat[1]=myRound(disp_data_buffWBf);
																	}	
																	//ESP_LOGI(TAG,"Recv trgt:%f",dataType->valueint);
																	disp_static=4;
																	exit_time=0;
																	disp_statu=SET_MEAT;    // 设定肉探针温度
																	adjust_index=6;
																	beencnt=10;
																	clear_setmeat();
																	f_first_over1=0;
																	meat_probe_channl=1;
																	setmeat_probe_ok[1]=1;
																}
														}
												}
										}

										if(cJSON_HasObjectItem(meatProbes,"p3"))
										{
											cJSON *probe3 = cJSON_GetObjectItem(meatProbes,"p3");
											if(probe3!=NULL)
												{
													if(cJSON_HasObjectItem(probe3,"temp"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(probe3,"temp");
															//act_meat_temp[2]=dataType->valuedouble;
															//ESP_LOGI(TAG,"Recv temp:%f",dataType->valuedouble);
															b_desired=1;
														}
													
													if(cJSON_HasObjectItem(probe3,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(probe3,"trgt");
															b_desired=1;
															if(meat_probe_err[2]==0)
																{
																	disp_data_buffWB=dataType->valueint;
																	if(f_c_f==0) //F
																	{
																		set_temp_meat[2]=disp_data_buffWB;
																	}
																	else
																	{
																		disp_data_buffWBf=disp_data_buffWB;
																		disp_data_buffWBf=(disp_data_buffWBf*9)/5+32;
																		set_temp_meat[2]=myRound(disp_data_buffWBf);
																	}	
																	//ESP_LOGI(TAG,"Recv trgt:%d",dataType->valueint);
																	disp_static=5;
																	exit_time=0;
																	disp_statu=SET_MEAT;    // 设定肉探针温度
																	adjust_index=6;
																	beencnt=10;
																	clear_setmeat();
 																	f_first_over2=0;
																	meat_probe_channl=2;
																	setmeat_probe_ok[2]=1;
																}
														}
												}
										}

										if(cJSON_HasObjectItem(meatProbes,"p4"))
										{
											cJSON *probe4 = cJSON_GetObjectItem(meatProbes,"p4");
											if(probe4!=NULL)
												{
													if(cJSON_HasObjectItem(probe4,"temp"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(probe4,"temp");
															//act_meat_temp[3]=dataType->valuedouble;
															//ESP_LOGI(TAG,"Recv temp:%f",dataType->valuedouble);
															b_desired=1;
														}
													
													if(cJSON_HasObjectItem(probe4,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(probe4,"trgt");
															b_desired=1;
															if(meat_probe_err[3]==0)
																{
																	disp_data_buffWB=dataType->valueint;
																	if(f_c_f==0) //F
																	{
																		set_temp_meat[3]=disp_data_buffWB;
																	}
																	else
																	{
																		disp_data_buffWBf=disp_data_buffWB;
																		disp_data_buffWBf=(disp_data_buffWBf*9)/5+32;
																		set_temp_meat[3]=myRound(disp_data_buffWBf);
																	}	
																	//ESP_LOGI(TAG,"Recv trgt:%f",dataType->valueint);
																	disp_static=6;
																	exit_time=0;
																	disp_statu=SET_MEAT;    // 设定肉探针温度
																	adjust_index=6;
 																	f_first_over3=0;
																	beencnt=10;
																	clear_setmeat();
																	meat_probe_channl=3;
																	setmeat_probe_ok[3]=1;
																}
														}
												}
										}

									}
							}
							
							if(cJSON_HasObjectItem(state,"errors"))
								{
									cJSON *errorCodes = cJSON_GetObjectItem(state,"errors");
									//ESP_LOGI(TAG,"Recv errorCodes:%d",errorCodes->valueint);
									if(errorCodes!=NULL)
										{
											//arry_index=cJSON_GetArraySize(errorCodes);	
											//ESP_LOGI(TAG,"Recv GetArraySize:%d",arry_index);
											if(cJSON_GetArrayItem(errorCodes,0))
												{
													//grill_probe_err=errorCodes->valueint;
													ESP_LOGI(TAG,"Recv errors[0]:%d",errorCodes->valueint);
													//ESP_LOGI(TAG,"errors[0]:%f",errorCodes->valuedouble);
													b_desired=1;
												}

											if(cJSON_GetArrayItem(errorCodes,1))
												{
													//ER2=errorCodes->valueint;
													//ESP_LOGI(TAG,"Recv errors[1]:%d",errorCodes->valueint);
													b_desired=1;
												}

											if(cJSON_GetArrayItem(errorCodes,2))
												{
													//ER3=errorCodes->valueint;
													ESP_LOGI(TAG,"Recv errors[2]:%d",errorCodes->valueint);
													b_desired=1;
												}

											if(cJSON_GetArrayItem(errorCodes,3))
												{
													//ER4=errorCodes->valueint;
													ESP_LOGI(TAG,"Recv errors[3]:%d",errorCodes->valueint);
													b_desired=1;	
												}
											
											if(cJSON_GetArrayItem(errorCodes,4))
												{
													//ER5=errorCodes->valueint;
													ESP_LOGI(TAG,"Recv errors[4]:%d",errorCodes->valueint);
													b_desired=1;
												}
										}
								}
							
							if(cJSON_HasObjectItem(state,"timers"))
							{
								cJSON *timers = cJSON_GetObjectItem(state,"timers");
								if(timers!=NULL)
									{
										if(cJSON_HasObjectItem(timers,"up"))
										{
											cJSON *countUp = cJSON_GetObjectItem(timers,"up");
											if(countUp!=NULL)
												{
													if(cJSON_HasObjectItem(countUp,"paused"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(countUp,"paused");
															//btimer_pause=dataType->valueint;
															b_desired=1;
															//ESP_LOGI(TAG,"Recv isPaused:%d",dataType->valueint);
														}
													
													if(cJSON_HasObjectItem(countUp,"current"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(countUp,"current");
															//ESP_LOGI(TAG,"Recv current:%d",dataType->valueint);
															b_desired=1;
														}

													if(cJSON_HasObjectItem(countUp,"trgt"))
														{
															b_desired=1;
															//cJSON *dataType = cJSON_GetObjectItem(countUp,"trgt");
															//timer_all=dataType->valueint;
															//ESP_LOGI(TAG,"Recv trgt:%d",dataType->valueint);
														}
												}
										}
										
										if(cJSON_HasObjectItem(timers,"dwn"))
										{
											cJSON *countDown = cJSON_GetObjectItem(timers,"dwn");
											if(countDown!=NULL)
												{
													if(cJSON_HasObjectItem(countDown,"paused"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(countDown,"paused");
															//btimer_pause=dataType->valueint;
															b_desired=1;
															//ESP_LOGI(TAG,"Recv isPaused:%d",dataType->valueint);
														}
													
													if(cJSON_HasObjectItem(countDown,"current"))
														{
															//cJSON *dataType = cJSON_GetObjectItem(countDown,"current");
															//ESP_LOGI(TAG,"Recv current:%d",dataType->valueint);
															b_desired=1;
														}

													if(cJSON_HasObjectItem(countDown,"trgt"))
														{
															cJSON *dataType = cJSON_GetObjectItem(countDown,"trgt");
															timer_all=dataType->valueint;
															b_desired=1;
															timer_hour=timer_all/60;
															timer_min=timer_all%60;
															if(timer_alarmend==1)
															{
																b_beenon=1;
																ounce_count=0;
																timer_alarmend=0;
															}
															timer_hourbat=timer_hour;
															timer_minbat=timer_min;
															timer_secbat=0;
															beencnt=10;
															disp_static=1;
															exit_time=0;
															if(timer_hourbat!=0||timer_minbat!=0)
															{
																btimer_pause=0;
																timer_uplong=1;
																timer_alarm=1;
																ER4=0;
															}
															else
															{
																timer_alarm=0;
															}	
																
															ESP_LOGI(TAG,"Recv trgt:%d",dataType->valueint);
														}
												}
										}

									}
							}

							//if(cJSON_HasObjectItem(desired,"ssid"))
							//{
							//	cJSON *dataType = cJSON_GetObjectItem(desired,"ssid");
							//	ESP_LOGI(TAG,"Recv ssid:%s",dataType->valuestring);
							//}
							
						}
					}
				}
			}
			
		}

		//if(root)
			{
  				cJSON_Delete(root);
				root=NULL;
			}
	}
}
/****************************************影子服务程序********************************************/
// 更新接受请求
void iot_Update_accept_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
								 IoT_Publish_Message_Params *params, void *pData) {
	//ESP_LOGI(TAG, "update accept callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);
	ESP_LOGI(TAG, "updatey accept callback");
	dealAwsData( (char *)params->payload, params->payloadLen);
}
// 更新拒绝
void iot_Update_reject_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
							   IoT_Publish_Message_Params *params, void *pData) {
	ESP_LOGI(TAG, "update reject callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);

}
//  更新完成提示
void iot_Update_document_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
							   IoT_Publish_Message_Params *params, void *pData) {
	ESP_LOGI(TAG, "update document callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);

}
// 请求影子成功
void iot_get_accept_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
							   IoT_Publish_Message_Params *params, void *pData) {
	//ESP_LOGI(TAG, "get accept callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);
	showdow_callback=1;
	device_busy=0;
	ESP_LOGI(TAG, "get accept callback");
}

// 请求影子拒绝
void iot_get_reject_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
							   IoT_Publish_Message_Params *params, void *pData) {
	ESP_LOGI(TAG, "get reject callback, %.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);

}

//  订阅更新接受请求
char topicSusbicUpdateAccept[70];
/*
char subscribeUpdateAccept(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicUpdateAccept,0,70);
	sprintf(topicSusbicUpdateAccept,"$aws/things/%s/shadow/update/accepted",ThingName);
	topicLen = strlen(topicSusbicUpdateAccept);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicUpdateAccept);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicUpdateAccept, topicLen, QOS0, iot_Update_accept_handler, NULL);
    if(SUCCESS != rc)
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error1 subscribing %s: %d ",topicSusbicUpdateAccept,rc);
        return 0;
    }
	
	return 1;
}
*/

char subscribeUpdateAccept(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;
		
	unsigned int topicLen = 0;
	memset(topicSusbicUpdateAccept,0,70);
	sprintf(topicSusbicUpdateAccept,"$aws/things/%s/shadow/update/delta",ThingName);
	topicLen = strlen(topicSusbicUpdateAccept);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicUpdateAccept);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicUpdateAccept, topicLen, QOS0, iot_Update_accept_handler, NULL);
	if(SUCCESS != rc)
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
		ESP_LOGI(TAG, "Error1 subscribing %s: %d ",topicSusbicUpdateAccept,rc);
		return 0;
	}
      else
     {
	sub_time=100;
	Callback_count=6;
     }		
	return 1;
}

//  订阅更新拒绝
char topicSusbicUpdateReject[70];
char subscribeUpdateReject(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicUpdateReject,0,70);
	sprintf(topicSusbicUpdateReject,"$aws/things/%s/shadow/update/rejected",ThingName);
	topicLen = strlen(topicSusbicUpdateReject);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicUpdateReject);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicUpdateReject, topicLen, QOS0, iot_Update_reject_handler, NULL);
    if(SUCCESS != rc) 
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error2 subscribing %s: %d ",topicSusbicUpdateReject,rc);
        return 0;
    }
    else
    {
	sub_time=100;
	Callback_count=5;
    }
	return 1;
}

// 订阅更新完成
char topicSusbicUpdateDocument[70];
char subscribeUpdateDocument(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicUpdateDocument,0,70);
	sprintf(topicSusbicUpdateDocument,"$aws/things/%s/shadow/update/document",ThingName);
	topicLen = strlen(topicSusbicUpdateDocument);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicUpdateDocument);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicUpdateDocument, topicLen, QOS0, iot_Update_document_handler, NULL);
    if(SUCCESS != rc)
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error3 subscribing %s: %d ",topicSusbicUpdateDocument,rc);
        return 0;
    }
     else
    {
	sub_time=100;
	Callback_count=4;
    }
	return 1;
}

// 订阅请求影子成功
char topicSusbicUpdateGetAccept[70];
char subscribeGetAccept(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicUpdateGetAccept,0,70);
	sprintf(topicSusbicUpdateGetAccept,"$aws/things/%s/shadow/get/accepted",ThingName);
	topicLen = strlen(topicSusbicUpdateGetAccept);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicUpdateGetAccept);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicUpdateGetAccept, topicLen, QOS0, iot_get_accept_handler, NULL);
    if(SUCCESS != rc) 
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error7 subscribing %s: %d ",topicSusbicUpdateGetAccept,rc);
        return 0;
    }
     else
    {
	sub_time=100;
	Callback_count=3;
    }	
	return 1;
}

// 订阅影子拒绝
char topicSusbicUpdateGetReject[70];
char subscribeGetReject(AWS_IoT_Client *pClient)
{
	IoT_Error_t rc = FAILURE;

	unsigned int topicLen = 0;
	memset(topicSusbicUpdateGetReject,0,70);
	sprintf(topicSusbicUpdateGetReject,"$aws/things/%s/shadow/get/rejected",ThingName);
	topicLen = strlen(topicSusbicUpdateGetReject);
	ESP_LOGI(TAG,"SubscrTopic:%s",topicSusbicUpdateGetReject);
	rc = aws_iot_mqtt_subscribe(pClient, topicSusbicUpdateGetReject, topicLen, QOS0, iot_get_reject_handler, NULL);
    if(SUCCESS != rc)
	{
		isErrorSubscribeShadow=1;
		//isCallbackFlag=0;
        ESP_LOGI(TAG, "Error4 subscribing %s: %d ",topicSusbicUpdateGetReject,rc);
        return 0;
    }
     else
    {
	sub_time=100;
	Callback_count=2;
    }	
	return 1;
}

// 获取影子
char topicUpdateGet[70];
char topicUpdateGetMsgBuf[70];
char sendRequestGet(AWS_IoT_Client *pClient)
{
	
	memset(topicUpdateGet,0,70);
	memset(topicUpdateGetMsgBuf,0,70);
	sprintf(topicUpdateGet,"$aws/things/%s/shadow/get",ThingName);
	ESP_LOGI(TAG,"Request Get:%s",topicUpdateGet);
	IoT_Publish_Message_Params publishParams;
	publishParams.qos = QOS0;
	publishParams.isRetained = 0;
	publishParams.isDup = 0;
	publishParams.id = 0;
	publishParams.payload = topicUpdateGetMsgBuf;
	publishParams.payloadLen = 0;

	return aws_iot_mqtt_publish(pClient, topicUpdateGet, strlen(topicUpdateGet), &publishParams);	
	
}

// 更新状态
char topicUpdateStatus[70];
char updateStatusBuf[2236];
char sendRequestUpdateStatus(AWS_IoT_Client *pClient,char *msg,unsigned int len)
{
	
	memset(topicUpdateStatus,0,70);
	memset(updateStatusBuf,0,2236);
	memcpy(updateStatusBuf,msg,len);
	sprintf(topicUpdateStatus,"$aws/things/%s/shadow/update",ThingName);
	ESP_LOGI(TAG,"Update Status:%s   Msg:%s",topicUpdateStatus,updateStatusBuf);
	ESP_LOGI(TAG,"Update Status length=%d",len);
	IoT_Publish_Message_Params publishParams;
	publishParams.qos = QOS0;
	publishParams.isRetained = 0;
	publishParams.isDup = 0;
	publishParams.id = 0;
	publishParams.payload = updateStatusBuf;
	publishParams.payloadLen = len;

	return aws_iot_mqtt_publish(pClient, topicUpdateStatus, strlen(topicUpdateStatus), &publishParams);	
	
}

int error[5] = {0,0,0,0,0};
void sendTestData(void)
{
	HeatSource hs[4];
	MeatProbes mp[4];
	MbTimers tm[2];
	uint8_t timer_mode=0;
	uint8_t meat_probe=0;
	uint8_t UsingFah=0;

	if(f_c_f==0)
		{
			UsingFah=1;
		}
	else
		{
			UsingFah=0;
		}
	
	if(grill_probe_err==1)
		{
			error[0]=1;
		}
	else
		{
			error[0]=0;
		}

		if(meat_probe_errshort[0]==1||meat_probe_errshort[1]==1||meat_probe_errshort[2]==1
		||meat_probe_errshort[3]==1)  
		{
			error[1]=2;
		} 
	else
		{
			error[1]=0;
		}
	
	if(ER3==1) 
		{
			error[2]=3;
		}  
	else
		{
			error[2]=0;
		}

	 if(ER4==1) 
		{
			error[3]=4;
		}  
	 else
	 	{
		  	error[3]=0;
	 	}
			
	if(ER5==1) 
		{
			error[4]=5;
		}
	else
		{
			error[4]=0;
		}


	if(timer_alarm==1)
		{
			if(count_up_domwn==0)
			{
				timer_mode=0x02;
			}
			else
			{
				timer_mode=0x01;
			}
		}
	else
		{
			timer_mode=0x00;
		}	
	
	//update_static=0;
	if(upgradeStepNow()==1)
		{
			update_static=1;	//  Checking for updates"
		}
	
	if(upgradeStepNow()==2)
		{
			update_static=2;	// Update available"
		}

	if(upgradeStepNow()==3)
		{
			update_static=3;	// "3: Downloading and applying update"
		}

	if(close_blue_ok==1)
		{
			if(upgradeStepNow()==4)
			{
				update_static=0;	// "4: update  OK"
			}
	
			if(upgradeStepNow()==5)
			{
				update_static=0;	// "5: update  FAILD"
			}
		}
	
	tm[0].isPaused = btimer_pause;  //  "countUp": 
	tm[0].type =  1;
	tm[0].currentValue =  timer_hourUP*60+timer_minUP;
	tm[0].setTargetTime =24*60;
	
	tm[1].isPaused = btimer_pause;  // "countDown": 
	tm[1].type =  2;
	tm[1].currentValue = timer_hourbat*60+timer_minbat;
	tm[1].setTargetTime =  timer_hour*60+timer_min;
		
	if(meat_probe_err[0]==0)
		{
			mp[0].probeNumber = 1;
			mp[0].currentTemperature = act_meat_temp[0];
			mp[0].targetTemperature = set_temp_meat[0];
			meat_probe|=0x01;
		}

	if(meat_probe_err[1]==0)
		{
			mp[1].probeNumber = 2;
			mp[1].currentTemperature =  act_meat_temp[1];
			mp[1].targetTemperature =  set_temp_meat[1];
			meat_probe|=0x02;
		}

	if(meat_probe_err[2]==0)
		{
			mp[2].probeNumber = 3;
			mp[2].currentTemperature = act_meat_temp[2];
			mp[2].targetTemperature = set_temp_meat[2];
			meat_probe|=0x04;
		}

	if(meat_probe_err[3]==0)
		{
			mp[3].probeNumber = 4;
			mp[3].currentTemperature =act_meat_temp[3];
			mp[3].targetTemperature = set_temp_meat[3];
			meat_probe|=0x08;
		}
	
	hs[0].type = 1;
	hs[0].isHeating = false;
	hs[0].setTargetLevel = set_temp_grill;
	hs[0].currentLevel = act_grill_temp;
	hs[0].maxLevel = max_set_temp;
	hs[0].minLevel = min_set_temp;
	
	hs[1].type = 2;
	if(set_temp_ok==1)
		{
			hs[1].isHeating = true;
		}
	hs[1].setTargetLevel = set_temp_grill;
	hs[1].currentLevel = act_grill_temp;
	hs[1].maxLevel = max_set_temp;
	hs[1].minLevel = min_set_temp;
	
	hs[2].type = 3;
	hs[2].isHeating = false;
	hs[2].setTargetLevel = set_temp_grill;
	hs[2].currentLevel = act_grill_temp;
	hs[2].maxLevel = max_set_temp;
	hs[2].minLevel = min_set_temp;

	hs[3].type = 4;
	hs[3].isHeating = false;
	hs[3].setTargetLevel = set_temp_grill;
	hs[3].currentLevel = act_grill_temp;
	hs[3].maxLevel = max_set_temp;
	hs[3].minLevel = min_set_temp;
	
	setAwsData("C:G:P24:1:D",b_power_on,UsingFah,wifi_shown_contrl,open_door,act_grill_temp,hs,0x02,mp,meat_probe,tm,timer_mode,error,5);
	
}

////////////////////////////////////////////
unsigned int controlTimeCount = 0;
unsigned char xcount = 0;
char controlMethon(AWS_IoT_Client *pClient)
{
		xcount++;
		if(xcount>10||b_desired==1)
		{
			xcount = 0;
			if((controlTimeCount%3==0||b_desired==1)&&upgradeStepNow()!=3&&wifi_connect==1)
			{
				sendTestData();
				b_desired=0;
			}
			
			if(controlTimeCount==0&&upgradeStepNow()!=3)
			{
				//	发送一次获取状态
				sendRequestGet(pClient);
			}
			controlTimeCount++;
			if(controlTimeCount>600)
			{
				controlTimeCount = 0;
			}
			
		}
		return 0;
}

#if 1
char jsonSendBuffer[2236];
void setAwsData(char *serialNumber,char isDeviceOn,char isUsingFahrenheit,char isBLEorWiFiControlEngaged,
				char isLidOrDoorOpen,int mainProbeTemperature,HeatSource *heatSource,unsigned char hsl,
				MeatProbes *meatProbes,unsigned char mpl, MbTimers *timers,unsigned char tl,int *error,unsigned char el)
{
		char IoT_data_t;
		IoT_Error_t rc = FAILURE;
		memset(jsonSendBuffer,0,2236);
		
		cJSON *aws = cJSON_CreateObject();
		
		cJSON *state= cJSON_CreateObject();
		
		
		cJSON *root = NULL;
		//cJSON *root2= NULL;
		cJSON *root2 = cJSON_CreateNull();
		//if(b_desiredinit==1)
		{
			//root2 = cJSON_CreateObject();
		}
		root = cJSON_CreateObject();
		
		
		cJSON_AddStringToObject(root, "model", serialNumber);
		cJSON_AddStringToObject(root, "vers", (const char *)update_ver);
		cJSON_AddNumberToObject(root,"update",update_static);
		cJSON_AddNumberToObject(root,"trigger",updateTrigger);
		
		//cJSON_AddStringToObject(root, "macAddress", (const char*)targerbuf); //30 ae a4 9f 81 1e 
		cJSON_AddStringToObject(root, "mac", (const char *)BlueMacStringTemp); //30 ae a4 9f 81 1e
		cJSON_AddBoolToObject(root,"pwrOn",(const cJSON_bool)isDeviceOn);
		cJSON_AddBoolToObject(root,"fah",(const cJSON_bool)isUsingFahrenheit);
		
		cJSON_AddBoolToObject(root,"engaged",(const cJSON_bool)isBLEorWiFiControlEngaged);
		cJSON_AddBoolToObject(root,"doorOpn",(const cJSON_bool)isLidOrDoorOpen);


		if(isUsingFahrenheit==1)//F
		{
			cJSON_AddNumberToObject(root,"mainTemp",mainProbeTemperature);
		
		}
		else
		{
			disp_data_buffWB=(mainProbeTemperature-32)*5/9; 
			cJSON_AddNumberToObject(root,"mainTemp",disp_data_buffWB);
		
		}
		if(hsl>0)
		{
			cJSON *j_hs = cJSON_CreateObject();
			if((hsl&0x01)==0x01)
			{
				cJSON *c_hs1 = cJSON_CreateObject();
				//cJSON_AddNumberToObject(c_hs1, "type", heat[i].type);
				cJSON_AddBoolToObject(c_hs1, "heating", false);
				cJSON_AddNumberToObject(c_hs1, "trgt", set_temp_grill);
				cJSON_AddNumberToObject(c_hs1, "intensity", act_grill_temp);
				cJSON_AddNumberToObject(c_hs1, "max", max_set_temp);
				cJSON_AddNumberToObject(c_hs1, "min", min_set_temp);
				cJSON_AddItemToObject(j_hs,"t1",c_hs1);
			}
			
			if((hsl&0x02)==0x02)
			{
				cJSON *c_hs2 = cJSON_CreateObject();
				//cJSON_AddNumberToObject(c_hs2, "type", heat[i].type);
				if(set_temp_ok==1)
					{
						cJSON_AddBoolToObject(c_hs2, "heating", true);
						cJSON_AddNumberToObject(c_hs2, "intensity",(psArgument_pwmCycle*100/16383));
					}
				else
					{
						cJSON_AddBoolToObject(c_hs2, "heating", false);
						cJSON_AddNumberToObject(c_hs2, "intensity",0);
					}

				if(f_c_f==0)//F
					{
						cJSON_AddNumberToObject(c_hs2, "trgt", set_temp_grill);
						cJSON_AddNumberToObject(c_hs2, "max", max_set_temp);
						cJSON_AddNumberToObject(c_hs2, "min", min_set_temp);

					}
				else
					{
						disp_data_buffWB=(set_temp_grill-32)*5/9; 
						cJSON_AddNumberToObject(c_hs2, "trgt", disp_data_buffWB);

						disp_data_buffWB=(max_set_temp-32)*5/9; 
						cJSON_AddNumberToObject(c_hs2, "max", disp_data_buffWB);

						disp_data_buffWB=(min_set_temp-32)*5/9; 
						cJSON_AddNumberToObject(c_hs2, "min", disp_data_buffWB);

					}
				cJSON_AddItemToObject(j_hs,"t2",c_hs2);
			}
			
			if((hsl&0x04)==0x04)
			{
				cJSON *c_hs3 = cJSON_CreateObject();
				//cJSON_AddNumberToObject(c_hs3, "type", heat[i].type);
				cJSON_AddBoolToObject(c_hs3, "heating", false);
				cJSON_AddNumberToObject(c_hs3, "trgt", 3);
				cJSON_AddNumberToObject(c_hs3, "intensity", 3);
				cJSON_AddNumberToObject(c_hs3, "max", 3);
				cJSON_AddNumberToObject(c_hs3, "min", 3);
				cJSON_AddItemToObject(j_hs,"t3",c_hs3);
			}
			if((hsl&0x08)==0x08)
			{
				cJSON *c_hs4 = cJSON_CreateObject();
				//cJSON_AddNumberToObject(c_hs4, "type", heat[i].type);
				cJSON_AddBoolToObject(c_hs4, "heating", false);
				cJSON_AddNumberToObject(c_hs4, "trgt", 3);
				cJSON_AddNumberToObject(c_hs4, "intensity", 3);
				cJSON_AddNumberToObject(c_hs4, "max", 3);
				cJSON_AddNumberToObject(c_hs4, "min", 3);
				cJSON_AddItemToObject(j_hs,"t4",c_hs4);
			}
			cJSON_AddItemToObject(root,"heat",j_hs);
		}
		else
		{	
			cJSON_AddStringToObject(root, "heat", "[]");
		}
		
		if(mpl>0)
		{
			cJSON *j_mp = cJSON_CreateObject();
			if((mpl&0x01) ==0x01)
			{
				cJSON *c_mp1 = cJSON_CreateObject();
				//cJSON_AddNumberToObject(c_mp, "probeNumber", meatProbes[i].probeNumber);
				
				if(isUsingFahrenheit==1)//F
				{
					cJSON_AddNumberToObject(c_mp1, "temp", act_meat_temp[0]);
		
				}
				else
				{
					disp_data_buffWB=(act_meat_temp[0]-32)*5/9; 
					cJSON_AddNumberToObject(c_mp1, "temp", disp_data_buffWB);
		
				}
				
				if(setmeat_probe_ok[0]==1)
					{
						if(isUsingFahrenheit==1)//F
						{
							cJSON_AddNumberToObject(c_mp1, "trgt", set_temp_meat[0]);
		
						}
						else
						{
							disp_data_buffWB=(set_temp_meat[0]-32)*5/9; 
							cJSON_AddNumberToObject(c_mp1, "trgt", disp_data_buffWB);
		
						}
				
					}
				else
					{
						cJSON_AddNumberToObject(c_mp1, "trgt", 0);
					}
				
				cJSON_AddItemToObject(j_mp,"p1",c_mp1);
			}
			else
			{
				cJSON *c_mp1 = cJSON_CreateNull();
				cJSON_AddItemToObject(j_mp,"p1",c_mp1);
			}
			
			if((mpl&0x02) ==0x02)
			{
				cJSON *c_mp2 = cJSON_CreateObject();
				if(isUsingFahrenheit==1)//F
				{
					cJSON_AddNumberToObject(c_mp2, "temp", act_meat_temp[1]);
		
				}
				else
				{
					disp_data_buffWB=(act_meat_temp[1]-32)*5/9; 
					cJSON_AddNumberToObject(c_mp2, "temp", disp_data_buffWB);
		
				}
				
				if(setmeat_probe_ok[1]==1)
					{
						if(isUsingFahrenheit==1)//F
						{
							cJSON_AddNumberToObject(c_mp2, "trgt", set_temp_meat[1]);
		
						}
						else
						{
							disp_data_buffWB=(set_temp_meat[1]-32)*5/9; 
							cJSON_AddNumberToObject(c_mp2, "trgt", disp_data_buffWB);
						}
					}
				else
					{
						cJSON_AddNumberToObject(c_mp2, "trgt", 0);
					}
					
				cJSON_AddItemToObject(j_mp,"p2",c_mp2);
			}
			else
			{
				cJSON *c_mp2 = cJSON_CreateNull();
				cJSON_AddItemToObject(j_mp,"p2",c_mp2);
			}
			
			if((mpl&0x04) ==0x04)
			{
				cJSON *c_mp3 = cJSON_CreateObject();
				
				if(isUsingFahrenheit==1)//F
				{
					cJSON_AddNumberToObject(c_mp3, "temp",act_meat_temp[2]);
		
				}
				else
				{
					
					disp_data_buffWB=(act_meat_temp[2]-32)*5/9; 
					cJSON_AddNumberToObject(c_mp3, "temp",disp_data_buffWB);
					
				}
				if(setmeat_probe_ok[2]==1)
					{
						if(isUsingFahrenheit==1)//F
						{
							cJSON_AddNumberToObject(c_mp3, "trgt", set_temp_meat[2]);
		
						}
						else
						{
							disp_data_buffWB=(set_temp_meat[2]-32)*5/9; 
							cJSON_AddNumberToObject(c_mp3, "trgt", disp_data_buffWB);
						}
					}
				else
					{
						cJSON_AddNumberToObject(c_mp3, "trgt",0);
					}
				cJSON_AddItemToObject(j_mp,"p3",c_mp3);
			}
			else
			{
				cJSON *c_mp3 = cJSON_CreateNull();
				cJSON_AddItemToObject(j_mp,"p3",c_mp3);
			}
			
			if((mpl&0x08) ==0x08)
			{
				cJSON *c_mp4 = cJSON_CreateObject();
				if(isUsingFahrenheit==1)//F
				{
					cJSON_AddNumberToObject(c_mp4, "temp", act_meat_temp[3]);
		
				}
				else
				{
					disp_data_buffWB=(act_meat_temp[3]-32)*5/9; 
					cJSON_AddNumberToObject(c_mp4, "temp", disp_data_buffWB);
				}
				if(setmeat_probe_ok[3]==1)
					{
						if(isUsingFahrenheit==1)//F
						{
							cJSON_AddNumberToObject(c_mp4, "trgt", set_temp_meat[3]);
						}
						else
						{
							disp_data_buffWB=(set_temp_meat[3]-32)*5/9; 
							cJSON_AddNumberToObject(c_mp4, "trgt", disp_data_buffWB);
						}
					}
				else
					{
						cJSON_AddNumberToObject(c_mp4, "trgt", 0);
					}
				cJSON_AddItemToObject(j_mp,"p4",c_mp4);
			}
			else
			{
				cJSON *c_mp4 = cJSON_CreateNull();
				cJSON_AddItemToObject(j_mp,"p4",c_mp4);
			}
			
			cJSON_AddItemToObject(root,"probes",j_mp);
		}
		else
		{
			cJSON *j_mp = cJSON_CreateNull();
			cJSON_AddItemToObject(root,"probes",j_mp);
			//cJSON_AddStringToObject(root, "probes", j_mp);
		}
	
		
		if(el>0)
			{
				cJSON *j_error = cJSON_CreateIntArray(error,el);
				cJSON_AddItemToObject(root,"errors",j_error);
			}
		else
			{
				cJSON_AddStringToObject(root, "errors", "[]");
			}
		
		if(tl>0)
			{
				cJSON *j_timer = cJSON_CreateObject();
					if((tl&0x01) ==0x01)
						{
							cJSON *c_timer_up = cJSON_CreateObject();
							cJSON_AddBoolToObject(c_timer_up, "paused", btimer_pause);
							cJSON_AddNumberToObject(c_timer_up, "current", timer_hourUP*60+timer_minUP);
							cJSON_AddNumberToObject(c_timer_up, "trgt", 24*60);
							cJSON_AddItemToObject(j_timer,"up",c_timer_up);
						}
					else
						{
							cJSON *c_timer_up = cJSON_CreateNull();
							cJSON_AddItemToObject(j_timer,"up",c_timer_up);

							//cJSON_DeleteItemFromObject(j_timer,"up");
						}
					
					if((tl&0x02) ==0x02)
						{
							cJSON *c_timer_down = cJSON_CreateObject();
							cJSON_AddBoolToObject(c_timer_down, "paused", btimer_pause);
							cJSON_AddNumberToObject(c_timer_down, "current", timer_hourbat*60+timer_minbat);
							cJSON_AddNumberToObject(c_timer_down, "trgt", timer_hour*60+timer_min);
							cJSON_AddItemToObject(j_timer,"dwn",c_timer_down);
						}
					else
						{
	                        cJSON *c_timer_down = cJSON_CreateNull();					
							cJSON_AddItemToObject(j_timer,"dwn",c_timer_down);
						}
					
						cJSON_AddItemToObject(root,"timers",j_timer);
				
			}
		else
			{
				cJSON *j_timer = cJSON_CreateNull();
				cJSON_AddItemToObject(root,"timers",j_timer);
				//cJSON_AddStringToObject(root, "timers", j_timer);
			}
	
		cJSON_AddStringToObject(root, "ssid", (const char*)xssid);

		//cJSON *Grssi = cJSON_CreateObject();
		cJSON_AddNumberToObject(root, "RSSI", wifidata.rssi);
		
		cJSON_AddItemToObject(state,"reported",root);	//	   reported


		//cJSON_AddItemToObject(state,"desired",root2);	//	desired
		//ESP_LOGI(TAG, "desired------");
		//////////////////////
		/*
		if(b_desired==1&&b_desiredinit==1&&b_desiredinitbat==0)
			{
				b_desired=0;
				b_desiredinitbat=1;
				cJSON_AddItemToObject(state,"desired",root2);	//	desired
				ESP_LOGI(TAG, "desired------");
			}
		else  if(b_desiredinitbat==1)
			{
				b_desiredinitbat=0;
				cJSON_Delete(root2);
				cJSON_DeleteItemFromObject(state,"desired");
				ESP_LOGI(TAG, "desired--Delete----");
			}
		*/
		//if(b_desiredinit==1)
		{
			cJSON_AddItemToObject(state,"desired",root2);	//	desired
			ESP_LOGI(TAG, "desired------");
		}
		cJSON_AddItemToObject(aws,"state",state);
		memset(jsonSendBuffer,0,2236);
		
		char *out = cJSON_Print(aws);
		strcpy(jsonSendBuffer,out);

		cJSON_Delete(aws);//释放内存
		free(out);
		//用mqtt、发出去
		IoT_data_t=sendRequestUpdateStatus(&client,jsonSendBuffer,strlen(jsonSendBuffer));
		if(IoT_data_t==0)
			{
				//if(showdow_callback==1)
					{
						isCOnnectAWSShadow=1;
						isConnectAWS=1;
						showdow_callback=1;
						device_busy=0;
						isErrorConnectAWS=0;
						isErrorSubscribeShadow=0;
						sendcount=0;
						b_desiredinit=1;
					}
			}
		else 
			{
				if(++sendcount>10)   // aws 连接出错
					{
						sendcount=10;
						isCOnnectAWSShadow=0;
						isConnectAWS=0;
						device_busy=0;
						showdow_callback=0;
						isErrorConnectAWS=1;
						isErrorSubscribeShadow=1;
						b_desiredinitbat=0;
						b_desiredinit=0;
						if(wifi_connect==1)
						{
							rc = aws_iot_mqtt_connect(&client, &connectParams);
       						if(SUCCESS != rc)
							{
								//isErrorConnectAWS=1;  //Can’t connect to AWS    -0x6800  /**< The operation timed out. */
            							//ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
								sendcount=6;
								ESP_LOGI(TAG, "Connected to AWS fail");
        						}
							else
							{
								isErrorConnectAWS=0;
								ESP_LOGI(TAG, "Connected to AWS ok");
								isCallbackFlag=0;
								Callback_count=0;
							}

							rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
   						 	if(SUCCESS != rc) 
							{
       							ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc);
   							}
							else 
							{
								sendcount=0;
								ESP_LOGE(TAG, "able to set Auto Reconnect to true - %d", rc);
							}
						}
					}
			}
			
		ESP_LOGI(TAG, "IoT_data_tsend %d",IoT_data_t);
	}
#endif

