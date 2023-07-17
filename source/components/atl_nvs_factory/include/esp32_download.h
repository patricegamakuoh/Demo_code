#ifndef _ESP32_OTA_DOWNLOAD
#define _ESP32_OTA_DOWNLOAD

#include <esp_http_client.h>



esp_err_t esp_https_ota(const esp_http_client_config_t *config,unsigned int len,float *progress);



#endif
