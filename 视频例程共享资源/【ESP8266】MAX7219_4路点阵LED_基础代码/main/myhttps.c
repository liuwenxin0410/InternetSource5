/* HTTPS GET Example using plain mbedTLS sockets
 *
 * Contacts the howsmyssl.com API via TLS v1.2 and reads a JSON
 * response.
 *
 * Adapted from the ssl_client1 example in mbedtls.
 *
 * Original Copyright (C) 2006-2016, ARM Limited, All Rights Reserved, Apache 2.0 License.
 * Additions Copyright (C) Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD, Apache 2.0 License.
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_tls.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "cJSON.h"

#include "oled.h"

#define WEB_SERVER "api.bilibili.com"
#define WEB_PORT 443
#define WEB_URL "https://api.bilibili.com/x/relation/stat?vmid=285767116"
#define WEB_GET_PATH "/x/relation/stat?vmid=285767116"


static const char *TAG = "HTTPS";

static const char *REQUEST = "GET " WEB_GET_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
    "\r\n";


/* Root cert for howsmyssl.com, taken from server_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const uint8_t bilibili_root_cert_pem_start[] asm("_binary_bilibili_root_cert_pem_start");
extern const uint8_t bilibili_root_cert_pem_end[] 	asm("_binary_bilibili_root_cert_pem_end");

static void https_get_task(void *pvParameters)
{
    char recv_buf[512] = {0}, contect_buf[512] = {0};
    int ret, len;
		uint16_t contect_len = 0;
		
		
		OLED_Clear();
		OLED_Show_FollowerCh();
    while(1) {
				contect_len = 0; 
				bzero(contect_buf, sizeof(contect_buf));
				OLED_ShowString(0, 0, (uint8_t *)"Loading...", 8);

				esp_tls_cfg_t cfg = {
            .cacert_pem_buf  = bilibili_root_cert_pem_start,
            .cacert_pem_bytes = bilibili_root_cert_pem_end - bilibili_root_cert_pem_start,
        };
				
        struct esp_tls *tls = esp_tls_conn_http_new(WEB_URL, &cfg);
        
        if(tls != NULL) {
          ESP_LOGI(TAG, "Connection established...");
        } else {
          ESP_LOGE(TAG, "Connection failed...");
          goto exit;
        }
        
        size_t written_bytes = 0, req_len = strlen(REQUEST);
				printf("HTTPS REQ [%d]:\r\n%s", req_len, REQUEST);
        do {
					ret = esp_tls_conn_write(tls, 
					                       REQUEST + written_bytes, 
					                       req_len - written_bytes);
					if (ret >= 0) {
					  ESP_LOGI(TAG, "%d bytes written", ret);
					  written_bytes += ret;
					} else if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE){
					  ESP_LOGE(TAG, "esp_tls_conn_write  returned 0x%x", ret);
					  goto exit;
					}
        } while(written_bytes < req_len);

        ESP_LOGI(TAG, "Reading HTTP response...");
				char ch = 0, c_start = 0;
				
        do{
					len = sizeof(recv_buf) - 1;
					bzero(recv_buf, sizeof(recv_buf));
					ret = esp_tls_conn_read(tls, (char *)recv_buf, len);

					if (ret == ESP_TLS_ERR_SSL_WANT_READ  || ret == ESP_TLS_ERR_SSL_WANT_WRITE)
					  continue;

					if(ret < 0){
					  ESP_LOGE(TAG, "esp_tls_conn_read  returned -0x%x", -ret);
					  break;
					}

					if(ret == 0){
					  ESP_LOGI(TAG, "connection closed");
					  break;
					}

					len = ret;
					/* Print response directly to stdout as it is read */
					for(int i = 0; i < len; i++) {
						ch = recv_buf[i];
					  putchar(ch);
						if(0 == c_start && '{' == ch){
							c_start = 1;
						}
						if(1 == c_start){
							if(0x0D == ch || 0x0A == ch) continue;
							contect_buf[contect_len++] = ch;
						}
					}
        } while(1);

				ESP_LOGI(TAG, "JSON:%s", contect_buf);
				
				cJSON * root = NULL;
				const cJSON* data = NULL;
				const cJSON* follower = NULL;
				root = cJSON_Parse(contect_buf); 
	
				if (!root) {
					ESP_LOGE(TAG, "Error before: [%s]\n",cJSON_GetErrorPtr());
			  }else{
					data = cJSON_GetObjectItemCaseSensitive(root, "data");
					if (cJSON_IsObject(data)){
						follower = cJSON_GetObjectItemCaseSensitive(data, "follower");

						if (cJSON_IsNumber(follower)){
							ESP_LOGI(TAG, "follower:%d", follower->valueint);
							OLED_ShowString(0, 0, (uint8_t *)"          ", 8);
							OLED_Show_FollowerCnt(follower->valueint);
						}
					}


					cJSON_Delete(root);
				}
    exit:
        esp_tls_conn_delete(tls);    
        putchar('\n'); // JSON output doesn't have a newline at end
				static int request_count;
        ESP_LOGI(TAG, "Completed %d requests", ++request_count);

        for(int countdown = 3600; countdown >= 0; countdown--) {//30分钟刷新一次
            ESP_LOGI(TAG, "%d...", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Starting again!");
    }
}

void https_task_init(void)
{
	ESP_ERROR_CHECK(esp_netif_init());
}

void https_start_task()
{
	xTaskCreate(&https_get_task, "https_get_task", 8192, NULL, 5, NULL);
}

