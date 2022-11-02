#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/uart.h"

#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

#include "cJSON.h"

#include "mycfg.h"

static const char *TAG = "CFG";

extern appcfg_ctx_t stCfgCtx;
static esp_err_t set_cfg(char *key, void *value, int len)
{
	esp_err_t err = ESP_OK;
	nvs_handle out_handle;
	char *name = APPCFG_NAME;

	err = nvs_open(name, NVS_READWRITE, &out_handle);
	if (err != ESP_OK){
		ESP_LOGE(TAG, "nvs_open [%s] failed, err = 0x%x", name, err);
		goto exit;
	}

	err = nvs_set_blob(out_handle, key, value, len);
	if (err != ESP_OK){
		ESP_LOGE(TAG, "KEY [%s] set failed, err = 0x%x", key, err);
	}

	nvs_close(out_handle);
exit:
	return err;
}


static esp_err_t get_cfg(char *key, void *out, size_t *out_len)
{
	esp_err_t err = ESP_OK;
	nvs_handle out_handle;
	char *name = APPCFG_NAME;

	err = nvs_open(name, NVS_READWRITE, &out_handle);
	if (err != ESP_OK){
		ESP_LOGE(TAG, "nvs_open [%s] failed, err = 0x%x", name, err);
		goto exit;
	}

	err = nvs_get_blob(out_handle, key, out, out_len);
	if (err != ESP_OK){
		ESP_LOGE(TAG, "KEY [%s] get failed, err = 0x%x", key, err);
	}

	nvs_close(out_handle);
exit:
	return err;

}

esp_err_t app_cfg_set(appcfg_ctx_t *ctx)
{
	return set_cfg(APPCFG_KEY, ctx, sizeof(appcfg_ctx_t));
}

esp_err_t app_cfg_get(appcfg_ctx_t *ctx)
{
	esp_err_t err = ESP_OK;
	size_t len = sizeof(appcfg_ctx_t);

	memset(ctx, 0, sizeof(appcfg_ctx_t));

	err = get_cfg(APPCFG_KEY, ctx, &len);

	return err;
}

esp_err_t app_cfg_parse(char *pJsonStr)
{
	int ret = ESP_FAIL;
	cJSON * root = NULL;
	const cJSON* sn = NULL;
	const cJSON* ip = NULL;
	const cJSON* port = NULL;
	ESP_LOGI(TAG, "\npJsonStr: %s\n", pJsonStr);
	
	root = cJSON_Parse(pJsonStr); 
	
	if (!root) {
      ESP_LOGE(TAG, "Error before: [%s]\n",cJSON_GetErrorPtr());
  }else{
  	memset(&stCfgCtx, 0, sizeof(appcfg_ctx_t));
	  sn = cJSON_GetObjectItemCaseSensitive(root, "sn");
		if (cJSON_IsString(sn) && (sn->valuestring != NULL)){
			ESP_LOGI(TAG, "sn: %s", sn->valuestring);
			snprintf(stCfgCtx.acDevSN, sizeof(stCfgCtx.acDevSN), "%s", sn->valuestring);
		}else{
			cJSON_Delete(root);
			return ret;
		}

		ip = cJSON_GetObjectItemCaseSensitive(root, "ip");
		if (cJSON_IsString(ip) && (ip->valuestring != NULL)){
			ESP_LOGI(TAG, "ip: %s", ip->valuestring);
			snprintf(stCfgCtx.acHostIP, sizeof(stCfgCtx.acHostIP), "%s", ip->valuestring);
		}else{
			cJSON_Delete(root);
			return ret;
		}
		
	  port = cJSON_GetObjectItemCaseSensitive(root, "port");
		if (cJSON_IsNumber(port) && port->valueint != 0){
			ESP_LOGI(TAG, "port:%d", port->valueint);
			stCfgCtx.usHostPort = port->valueint;
			ret = ESP_OK;
		}	  
		cJSON_Delete(root);
	}
	return ret;
}

void app_uart_cfg(uint8_t loop)
{
	esp_err_t err = ESP_OK;
	char * cfgData[CFG_DATA_SZ+1] = {0};
	int len = 0;
  do{
      //{"sn":"18888881","ip":"192.168.43.1","port":9021}
      len = uart_read_bytes(UART_NUM_0, (uint8_t *)cfgData, CFG_DATA_SZ, 3000/portTICK_RATE_MS);
      // Write data back to the UART
      if(0 < len){
				cfgData[len] = 0;
				uart_write_bytes(UART_NUM_0, (const char *) cfgData, len);
				
				err = app_cfg_parse(cfgData);
				if(ESP_OK == err){
					stCfgCtx.szHostCfgFlag = 0x3F;
					printf("\n********** APP Cfg OK **********\n");
					printf("SN  :%s\n", stCfgCtx.acDevSN);
					printf("IP  :%s\n", stCfgCtx.acHostIP);
					printf("Port:%d\n", stCfgCtx.usHostPort);
					err = app_cfg_set(&stCfgCtx);
					if(ESP_OK == err){
						break;
					}
				}else{
					printf("\n********** APP Cfg Failed **********\n");
				}
      }
  }while(loop);

}


void json_test(void)
{
	char *string = NULL;
	cJSON *resolution = cJSON_CreateObject();
	if(NULL == resolution){ESP_LOGI(TAG, "json object is NULL"); return;}

	//cJSON_AddNumberToObject(resolution, "height", 100); // if added number will got NULL..do know why! currently sdk verion 3.4


	cJSON_AddStringToObject(resolution, "name", "Awesome 4K");

	
			
	string = cJSON_PrintUnformatted(resolution);
	if(NULL != string){
		ESP_LOGI(TAG, "json string:%s", string);
	}else{
		ESP_LOGI(TAG, "json string is NULL");
	}

	cJSON *name = cJSON_CreateString("Awesome 4888888K");
	cJSON_ReplaceItemInObject(resolution, "name", name);

	string = cJSON_PrintUnformatted(resolution);
	if(NULL != string){
		ESP_LOGI(TAG, "json string:%s", string);
	}else{
		ESP_LOGI(TAG, "json string is NULL");
	}
	
	cJSON_Delete(resolution);
}

