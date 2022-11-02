#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "mygpio.h"

static const char *TAG = "GPIO";
SYS_LED_STA sysSta = 0;
void my_gpio_init()
{
  gpio_config_t io_conf;
	gpio_pin_state defLevel = GPIO_PIN_RESET;
	
  //OUTPUT
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);

	gpio_set_level(GPIO_NUM_2, defLevel);
	gpio_set_level(GPIO_NUM_4, defLevel);
  gpio_set_level(GPIO_NUM_5, defLevel);
	gpio_set_level(GPIO_NUM_12, defLevel);
	
	gpio_set_level(GPIO_NUM_13, defLevel);
  gpio_set_level(GPIO_NUM_14, defLevel);
	gpio_set_level(GPIO_NUM_15, defLevel);
  gpio_set_level(GPIO_NUM_16, defLevel);
	
	ESP_LOGI(TAG, "GPIO Init done");
}

void my_gpio_write(gpio_num_t gpio_num, gpio_pin_state state)
{
	gpio_set_level(gpio_num, state);
}

void my_gpio_toggle(gpio_num_t gpio_num)
{
	int level = gpio_get_level(gpio_num);
	if(1 == level){
		level = 0;
	}else{
		level = 1;
	}
	gpio_set_level(gpio_num, level);
}

void my_gpio_handler(uint8_t cmd)
{
	ESP_LOGI(TAG, "GPIO Cmd %02X", cmd);
	if(cmd&0x01){
		gpio_set_level(GPIO_NUM_4, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_4, GPIO_PIN_RESET);
	}
	
	if(cmd&0x02){
		gpio_set_level(GPIO_NUM_5, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_5, GPIO_PIN_RESET);
	}

	if(cmd&0x04){
		gpio_set_level(GPIO_NUM_12, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_12, GPIO_PIN_RESET);
	}

	if(cmd&0x08){
		gpio_set_level(GPIO_NUM_13, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_13, GPIO_PIN_RESET);
	}

	if(cmd&0x10){
		gpio_set_level(GPIO_NUM_14, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_14, GPIO_PIN_RESET);
	}

	if(cmd&0x20){
		gpio_set_level(GPIO_NUM_15, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_15, GPIO_PIN_RESET);
	}
	
	if(cmd&0x40){
		gpio_set_level(GPIO_NUM_16, GPIO_PIN_SET);
	}else{
		gpio_set_level(GPIO_NUM_16, GPIO_PIN_RESET);
	}
}

uint8_t my_get_all_gpio_sta(void)
{
	uint8_t sta = 0;

	if(1 == gpio_get_level(GPIO_NUM_4)) sta |= 0x01;
	if(1 == gpio_get_level(GPIO_NUM_5)) sta |= 0x02;
	if(1 == gpio_get_level(GPIO_NUM_12)) sta |= 0x04;
	if(1 == gpio_get_level(GPIO_NUM_13)) sta |= 0x08;	
	if(1 == gpio_get_level(GPIO_NUM_14)) sta |= 0x10;
	if(1 == gpio_get_level(GPIO_NUM_15)) sta |= 0x20;
	if(1 == gpio_get_level(GPIO_NUM_16)) sta |= 0x40;

	return sta;
}

void my_set_sta_flag(SYS_LED_STA sta)
{
	sysSta = sta;
}

static void my_sta_gpio_task(void* parm)
{
	TickType_t delay = 1000; 
	while(1){
		//ESP_LOGI(TAG, "gpio proc...%d", sysSta);
		if(SYS_OK == sysSta){
			gpio_set_level(APP_STA_LED, GPIO_PIN_SET);
			delay = 1000;
		}else if(SYS_DOING_WIFI_CONNECT == sysSta){
			my_gpio_toggle(APP_STA_LED);
			delay = 1000;
		}else if(SYS_DOING_SMARTCFG == sysSta){
			my_gpio_toggle(APP_STA_LED);
			delay = 100;
		}else if(SYS_DOING_CONNECTION == sysSta){
			my_gpio_toggle(APP_STA_LED);
			delay = 500;
		}
		vTaskDelay(delay/ portTICK_RATE_MS);
	}

}

void start_gpio_task(void)
{
	my_gpio_init();
	xTaskCreate(my_sta_gpio_task, "sta_gpio_task", 2048, NULL, 0, NULL);
}


