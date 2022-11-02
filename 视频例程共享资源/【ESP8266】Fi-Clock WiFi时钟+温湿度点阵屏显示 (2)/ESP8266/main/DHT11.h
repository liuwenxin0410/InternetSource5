#ifndef __MY_DHT11_H_
#define __MY_DHT11_H_

#include "mygpio.h"

#define DHT11_DP_PIN		GPIO_NUM_5

#define DHT11_DP_Clr() 	gpio_set_level(DHT11_DP_PIN, GPIO_PIN_RESET)
#define DHT11_DP_Set() 	gpio_set_level(DHT11_DP_PIN, GPIO_PIN_SET)

#define DHT11_DP_In() 	gpio_get_level(DHT11_DP_PIN)

uint8_t DHT11_Read(uint8_t dataBuf[5]);
#endif


