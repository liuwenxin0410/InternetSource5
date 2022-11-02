/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "utils.h"

#include "cJSON.h"

#include "utils.h"
#include "myapp.h"
#include "mycfg.h"
#include "mygpio.h"

//#define HOST_IP_ADDR "172.18.0.248"
//#define PORT      9999

extern appcfg_ctx_t stCfgCtx;

static const char *TAG = "SocketClient";
static char rs_buffer[1024] = {0};//, data_buf[512] = {0};

cJSON *pHeartbeatJsonRoot = NULL;

void handle_msg(char *pJsonStr)
{
	cJSON * root = NULL;
	const cJSON* device = NULL;
	const cJSON* params = NULL;
	ESP_LOGI(TAG, "pJsonStr: %s\n", pJsonStr);
	
	root = cJSON_Parse(pJsonStr); 

	if (!root) {
      ESP_LOGE(TAG, "Error before: [%s]\n",cJSON_GetErrorPtr());
  }else{

		device = cJSON_GetObjectItemCaseSensitive(root, "driver");
		params = cJSON_GetObjectItemCaseSensitive(root, "params");
		
		if (cJSON_IsString(device) && (device->valuestring != NULL)){
			ESP_LOGI(TAG, "driver: %s\n", device->valuestring);

			if(0 == strcmp("GPIO", device->valuestring)){

				if (cJSON_IsNumber(params)){
					ESP_LOGI(TAG, "params:%02X\n", params->valueint);
					uint8_t gpioCmd = params->valueint;
					my_gpio_handler(gpioCmd);
				}
			}else if(0 == strcmp("OLED", device->valuestring)){

				if (cJSON_IsObject(params)){
					//ESP_LOGI(TAG, "params:%s\n", params->valueint);
				}
			}
		}
		cJSON_Delete(root);
	}
}

char *pack_heartbeat_json(void)
{
	char *out = NULL;
	uint8_t gpioSta = my_get_all_gpio_sta();
	char acTemp[6] = {0};
	cJSON *replacement = NULL;
	snprintf(acTemp, sizeof(acTemp), "%d", gpioSta);
	if(NULL == pHeartbeatJsonRoot){
		pHeartbeatJsonRoot = cJSON_CreateObject();
		ESP_LOGI(TAG, "SN:%s-%d", stCfgCtx.acDevSN, gpioSta);
		cJSON_AddStringToObject(pHeartbeatJsonRoot, "sn", stCfgCtx.acDevSN);
		cJSON_AddStringToObject(pHeartbeatJsonRoot, "gpio", acTemp); // can not add Number, will got NULL, dont kwon why!!! ,currently SDK is 3.4
	}else{
		replacement = cJSON_CreateString(acTemp);
   	if(NULL != replacement){
			cJSON_ReplaceItemInObject(pHeartbeatJsonRoot, "gpio", replacement);
   	}
	}
	if(NULL != pHeartbeatJsonRoot){
		/* formatted print */
		//out = cJSON_Print(pHeartbeatJsonRoot);
		out = cJSON_PrintUnformatted(pHeartbeatJsonRoot);
	}
	if(NULL != out){
		ESP_LOGI(TAG, "JSON:%s", out);
	}else{
		ESP_LOGI(TAG, "JSON is NULL");
	}
	
	return out;
}


static void tcp_client_task(void *pvParameters)
{
    //char addr_str[128];
    int addr_family = 0, sock = -1;
    int ip_protocol = 0, err = 0, rlen = 0, loopCnt = 20;
		char *pEchoMsg = NULL;
		while (1) {
				/*if(0 == gpio_get_level(GPIO_NUM_0)){
					stCfgCtx.szHostCfgFlag = 0xFF;
					app_cfg_set(&stCfgCtx);
					esp_restart();
				}*/
				loopCnt = 20;
				my_set_sta_flag(SYS_DOING_CONNECTION);				
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = inet_addr(stCfgCtx.acHostIP);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(stCfgCtx.usHostPort);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        //inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

        sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect %s:%d : errno %d", stCfgCtx.acHostIP, stCfgCtx.usHostPort, errno);
            close(sock);
						app_uart_cfg(0);
            continue;
        }
				my_set_sta_flag(SYS_OK);
				
        ESP_LOGI(TAG, "Successfully connected");
				struct timeval tv;
		    /* Set recv timeout of this fd as per config */
		    tv.tv_sec = 0;
		    tv.tv_usec = 500000;
		    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        while (1) {
						if(20 == loopCnt){
								loopCnt = 0;
								pEchoMsg = pack_heartbeat_json();
								if(NULL != pEchoMsg){
			            err = send(sock, pEchoMsg, strlen(pEchoMsg), 0); 
									free(pEchoMsg);
			            if (err < 0) {
			                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
			                break;
			            }
								}
						}
            rlen = recv(sock, rs_buffer, sizeof(rs_buffer) - 1, 0);
            // Error occured during receiving
            if (0 > rlen && 11 != errno) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else if(0 < rlen){
								my_gpio_write(GPIO_NUM_2, GPIO_PIN_RESET);
                rs_buffer[rlen] = 0; // Null-terminate whatever we received and treat like a string

								send(sock, rs_buffer, rlen, 0);
								
								handle_msg(rs_buffer);
								my_gpio_write(GPIO_NUM_2, GPIO_PIN_SET);
            }
						loopCnt++;

            //vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void socket_task_init(void)
{
	ESP_ERROR_CHECK(esp_netif_init());
}

void start_socket_task(void)
{
	xTaskCreate(tcp_client_task, "tcp_client_task", 4096, NULL, 5, NULL);
}


