#ifndef _AWS_IOT_
#define _AWS_IOT_


void initAWSIot(void);
void setAwsWiFiConnectd(void);
void resetAwsWiFiConnectd(void);


void tellServerOTAOk(void);



typedef struct HeatSource{
	unsigned int type;
	char isHeating;
	int setTargetLevel;
	unsigned int currentLevel;
	int maxLevel;
	int minLevel;
}HeatSource;


typedef struct MeatProbes{
	unsigned int probeNumber;
	double currentTemperature;
	int targetTemperature;
}MeatProbes;


typedef struct MbTimers{
	unsigned int type;
	char isPaused;
	unsigned int currentValue;
	unsigned int setTargetTime;
}MbTimers;

void setAwsData(char *serialNumber,char isDeviceOn,char isUsingFahrenheit,char isBLEorWiFiControlEngaged,
				char isLidOrDoorOpen,int mainProbeTemperature,HeatSource *heatSource,unsigned char hsl,
				MeatProbes *meatProbes,unsigned char mpl, MbTimers *timers,unsigned char tl,int *error,unsigned char el);


#endif


