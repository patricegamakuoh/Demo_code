#ifndef	_ESP32_UPGRADE_
#define _ESP32_UPGRADE_



void initUpgrade(void);

void setUpgradeWiFiOk(void);


//1 ��Ҫ  0 ����Ҫ
char isDeviceNeedUpgrade(void);

//��ʼ����		����0 ��ʼ����  ����1����������		
char startUpgrade(void);


//��ǰ��������  0 ����   1 ���汾��    2������   3�������� 
unsigned char upgradeStepNow(void);

//��ǰ�������� 0-1.0    �������ٷֱ�
float getUpgradeProgress(void);


void setUrlUpgrade(char *url,unsigned int binlen);









char simple_ota_task(char *upgradebuf,unsigned int filelen,float *progress);


#endif

