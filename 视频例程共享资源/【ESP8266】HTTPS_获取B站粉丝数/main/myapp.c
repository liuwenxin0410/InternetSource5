/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"

#include "driver/uart.h"

#include "myapp.h"
#include "mycfg.h"
#include "mygpio.h"
#include "mywifi.h"
#include "mysocket.h"

#include "oled.h"

appcfg_ctx_t stCfgCtx;

void app_main()
{
	
	ESP_ERROR_CHECK(nvs_flash_init());

      /* Set uart to 115200 */
  uart_config_t uart_config = {
          .baud_rate = 115200,
          .data_bits = UART_DATA_8_BITS,
          .parity    = UART_PARITY_DISABLE,
          .stop_bits = UART_STOP_BITS_1,
          .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

	ESP_ERROR_CHECK( uart_param_config(UART_NUM_0, &uart_config) );
  /* Install UART driver for interrupt-driven reads and writes */
  ESP_ERROR_CHECK( uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0) );

	vTaskDelay(3000 / portTICK_RATE_MS);

  /* Print informations */
  printf("\n**********APP Start**********\n");
	printf("|-Name     : %s\n",  APP_NAME);
  printf("|-Version  : %s\n",  APP_VERSION);
  printf("|-Build    : %s-%s\n",__DATE__, __TIME__);
  printf("ESP8266_RTOS_SDK version: %s\n", esp_get_idf_version());
  printf("Available heap size: 0x%08x\n",esp_get_free_heap_size());

	my_gpio_init();
	
	gpio_start_task();
	#if 0
	esp_err_t err = ESP_OK;
	uint8_t init_flag = 0;
	err = app_cfg_get(&stCfgCtx);		
	if(ESP_OK == err){
		if(0x3F != stCfgCtx.szHostCfgFlag){
			init_flag = 1;
		}else{
			printf("\nSN  :%s\n", stCfgCtx.acDevSN);
			printf("IP  :%s\n", stCfgCtx.acHostIP);
			printf("Port:%d\n", stCfgCtx.usHostPort);
		}
	}else{
		init_flag = 1;
	}

	if(1 == init_flag){
		printf("\nPlese init module!\n");
		my_gpio_write(GPIO_NUM_2, GPIO_PIN_RESET);	
		
		app_uart_cfg(1);
	}
	#endif
	OLED_Init();
	OLED_Clear();
	OLED_DrawLogo();
	OLED_Show_MyBilibili();
	client_task_init();
	wifi_init_sta();
	
	
}
