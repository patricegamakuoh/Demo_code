#ifndef	_ESP32_UPGRADE_
#define _ESP32_UPGRADE_



void initUpgrade(void);

void setUpgradeWiFiOk(void);


//1 需要  0 不需要
char isDeviceNeedUpgrade(void);

//开始升级		返回0 开始升级  返回1正在升级中		
char startUpgrade(void);


//当前升级步骤  0 空闲   1 检查版本中    2升级中   3升级结束 
unsigned char upgradeStepNow(void);

//当前升级进度 0-1.0    浮点数百分比
float getUpgradeProgress(void);


void setUrlUpgrade(char *url,unsigned int binlen);









char simple_ota_task(char *upgradebuf,unsigned int filelen,float *progress);


#endif

