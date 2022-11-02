/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "driver/gpio.h"

#include "lwip/apps/sntp.h"

#include "bitled.h"
#include "dht11.h"

static const char *TAG = "SNTP";
/*
 ntp.aliyun.com -- Alibaba public NTP "https://help.aliyun.com/document_detail/92704.html"
 0.cn.pool.ntp.org -- china pool ntp "https://www.pool.ntp.org/zone/cn"
*/
#define SNTP_SERVER  "ntp.aliyun.com"

static void initialize_sntp(void)
{
  ESP_LOGI(TAG, "Initializing SNTP");
	
	sntp_stop();
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
	
  sntp_setservername(0, SNTP_SERVER);
  sntp_init();
}

static void obtain_time(void)
{
	initialize_sntp();

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 150; // 15s

	BITLED_Show_SNTP_Action(1);// reset action
	ESP_LOGI(TAG, "Waiting for system time to be set...");
	while (timeinfo.tm_year < (2021 - 1900) && ++retry < retry_count) {
	  //ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
	  vTaskDelay(100 / portTICK_PERIOD_MS);
	  time(&now);
	  localtime_r(&now, &timeinfo);
		BITLED_Show_SNTP_Action(0);
	}
	ESP_LOGI(TAG, "System time set done... (%d/%d)", retry, retry_count);
}



static void sntp_clock_task(void *arg)
{
	time_t now;
	struct tm timeinfo;

	int  tm_sec = -1;		 /* minutes after the hour [0-59] */
	int  tm_min = -1;		 /* minutes after the hour [0-59] */
	int  tm_hour = -1; 	 /* hours since midnight [0-23] */

	uint8_t dht11TimCnt = 0, dht11ShowCnt = 0, dht11ShowFlag = 0;

	uint8_t dataBuf[5] = {0};
	
	time(&now);
  localtime_r(&now, &timeinfo);

  // Is time set? If not, tm_year will be (1970 - 1900).
  if (timeinfo.tm_year < (2021 - 1900)) {
      ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP[%s]", SNTP_SERVER);
      obtain_time();
  }
	
	// Set timezone to China Standard Time
	setenv("TZ", "CST-8", 1);
	tzset();
		
	while (1) {
	  // update 'now' variable with current time
	  
	  time(&now);
	  localtime_r(&now, &timeinfo);
		
	  if (timeinfo.tm_year < (2021 - 1900)) {
			ESP_LOGE(TAG, "The current date/time error");
			//esp_restart();
	  }else{
			
			dht11TimCnt++;
			if(50 == dht11TimCnt){ // dht11 show frequency 5s
				dht11ShowFlag = 1; dht11ShowCnt = 0;
			}else if(70 == dht11TimCnt){// dht11 show 2s
				dht11ShowFlag = 0;
				dht11TimCnt = 0;
				tm_sec = -1; tm_min = -1; tm_hour = -1;
			}

			if(1 == dht11ShowFlag){
				if(0 == dht11ShowCnt%10){// value refresh frequency 1s 
					DHT11_Read(dataBuf);
					BITLED_Show_DHT11(dataBuf);
				}
				dht11ShowCnt++;
			}else{
				if(tm_sec != timeinfo.tm_sec){
					tm_sec = timeinfo.tm_sec;
					BITLED_Clock_Show_Sec((uint8_t)timeinfo.tm_sec);
				}
				
				if(tm_min != timeinfo.tm_min){
					tm_min = timeinfo.tm_min;
					BITLED_Clock_Show_Min((uint8_t)timeinfo.tm_min);
				}

				if(tm_hour != timeinfo.tm_hour){
					if(-1 != tm_hour && 4 == timeinfo.tm_hour){
						ESP_LOGE(TAG, "initialize sntp again");
						initialize_sntp();
					}
					tm_hour = timeinfo.tm_hour;
					BITLED_Clock_Show_Hour((uint8_t)timeinfo.tm_hour);
					
				}
			}
	  }

		
	  vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void sntp_start_task()
{    // SNTP service uses LwIP, please allocate large stack space.
	xTaskCreate(sntp_clock_task, "sntp_clock_task", 2048, NULL, 10, NULL);
}

