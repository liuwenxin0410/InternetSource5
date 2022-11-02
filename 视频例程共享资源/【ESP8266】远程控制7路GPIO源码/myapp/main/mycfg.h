#ifndef _MY_CFG_H_
#define _MY_CFG_H_

#include "esp_system.h"
#include "esp_err.h"


#define CFG_DATA_SZ  	  128
#define CFG_DEV_SN_SZ  	16
#define CFG_HOST_IP_SZ  15


#define APPCFG_NAME        "appCfg"
#define APPCFG_KEY         "ctx"

typedef struct appcfg_ctx_s{
	char szHostCfgFlag;
	char acDevSN[CFG_DEV_SN_SZ + 1];
	char acHostIP[CFG_HOST_IP_SZ + 1];
	uint16_t usHostPort;
}appcfg_ctx_t;

esp_err_t app_cfg_set(appcfg_ctx_t *ctx);
esp_err_t app_cfg_get(appcfg_ctx_t *ctx);
esp_err_t app_cfg_parse(char *pJsonStr);
void app_uart_cfg(uint8_t loop);
#endif

