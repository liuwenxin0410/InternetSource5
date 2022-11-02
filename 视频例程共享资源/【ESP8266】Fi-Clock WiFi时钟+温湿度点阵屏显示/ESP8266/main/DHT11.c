#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"

#include "DHT11.h"

#define DHT_MAXCOUNT    100

static const char *TAG = "DHT11";

static uint8_t DHT11_Read_Bit(void)
{
	uint8_t bit = 0, UsFlag = 0;

	while (0 == DHT11_DP_In() && DHT_MAXCOUNT> UsFlag){usleep(1); UsFlag++;}
	usleep(60);

	if (1 == DHT11_DP_In()){
		bit = 1;
	}

	UsFlag = 0;
	while (1 == DHT11_DP_In() && DHT_MAXCOUNT > UsFlag){usleep(1); UsFlag++;}

	return bit;
}

static uint8_t DHT11_Read_Byte(void)
{
	uint8_t i;
	uint8_t data = 0;
	for (i=0; i<8; i++){
	  data <<= 1;
	  data |= DHT11_Read_Bit();
	}
	return data;
}

uint8_t DHT11_Read(uint8_t dataBuf[5])
{
	uint8_t i = 0;
	uint8_t UsFlag = 0;
	uint8_t tempBuf[5] = {0};
	
	gpio_set_direction(DHT11_DP_PIN, GPIO_MODE_OUTPUT);
	DHT11_DP_Set();
	vTaskDelay(250 / portTICK_RATE_MS);
	
	DHT11_DP_Clr();
	vTaskDelay(20 / portTICK_RATE_MS);//总线拉低必须大于18ms

	DHT11_DP_Set();
	gpio_set_direction(DHT11_DP_PIN, GPIO_MODE_INPUT);
	usleep(30);

	while(1 == DHT11_DP_In() && DHT_MAXCOUNT > UsFlag){usleep(1); UsFlag++;}
	if(UsFlag == DHT_MAXCOUNT) {ESP_LOGI(TAG, "DHT11 Not ready! Failed"); return 0;}
	
	UsFlag = 0;
	while(0 == DHT11_DP_In() && DHT_MAXCOUNT > UsFlag){usleep(1); UsFlag++;}
	if(UsFlag == DHT_MAXCOUNT) {ESP_LOGI(TAG, "DHT11 ACK L Failed"); return 0;}

	UsFlag = 0;
	while(1 == DHT11_DP_In() && DHT_MAXCOUNT > UsFlag){usleep(1); UsFlag++;}
	if(UsFlag == DHT_MAXCOUNT) {ESP_LOGI(TAG, "DHT11 ACK H Failed"); return 0;}
	
	for (i = 0; i < 5; i++){
	  tempBuf[i] = DHT11_Read_Byte();
	}
	while(0 == DHT11_DP_In());
	gpio_set_direction(DHT11_DP_PIN, GPIO_MODE_OUTPUT);
  DHT11_DP_Set();//DHT进入空闲状态
	
	ESP_LOGI(TAG, "%02X-%02X-%02X-%02X-%02X",tempBuf[0],tempBuf[1],tempBuf[2],tempBuf[3],tempBuf[4]);
	uint8_t checksum = tempBuf[0] + tempBuf[1] + tempBuf[2] + tempBuf[3];
	if (checksum == tempBuf[4]){
		memcpy(dataBuf, tempBuf, 5);
		ESP_LOGI(TAG, "%d.%d%%-%d.%d`C",tempBuf[0],tempBuf[1],tempBuf[2],tempBuf[3]);
	  return 1;
	}
	return 0;
}


