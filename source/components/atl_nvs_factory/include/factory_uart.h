#ifndef _Factory_Uart_
#define _Factory_Uart_

#define CaCerKeyMaxLen	2000
#define ThingNameUartMaxLen 34
#define MacEncryptionUartMaxLen 10

void initFactoryUart(void);
void initCaCerKey(void);

void writeAwsRootCaPem(char *ca,int len);
int readAwsRootCaPem(char *buf);

void writeCertificatePemCrt(char *cer,int len);
int readCertificatePemCrt(char *buf);

void writePrivatePemKey(char *key,int len);
int readPrivatePemKey(char *buf);

void writeThingNameUart(char *key,int len);
int readThingNameUart(char *buf);
int readmac_EncryptionUart(char *buf);
void writemac_EncryptionUart(char *key,int len);

void HexArrayToString(unsigned char *hexarray,int length,unsigned char *string);
void SHA256Create(char *input,unsigned char *output);


#endif
