#ifndef _MY_MAX7219_H_
#define _MY_MAX7219_H_

#include "myapp.h"
#include "mygpio.h"

#define MAX7219_COUNT			4


#define MAX7219_CLK_PIN		GPIO_NUM_12
#define MAX7219_DIN_PIN		GPIO_NUM_14
#define MAX7219_CS_PIN		GPIO_NUM_4

#define MAX7219_SCLK_Clr() 	gpio_set_level(MAX7219_CLK_PIN, GPIO_PIN_RESET)//SCL SPI
#define MAX7219_SCLK_Set() 	gpio_set_level(MAX7219_CLK_PIN, GPIO_PIN_SET)

#define MAX7219_SDIN_Clr() 	gpio_set_level(MAX7219_DIN_PIN, GPIO_PIN_RESET)//SDA SPI
#define MAX7219_SDIN_Set() 	gpio_set_level(MAX7219_DIN_PIN, GPIO_PIN_SET)

#define MAX7219_CS_Clr() 		gpio_set_level(GPIO_NUM_4, GPIO_PIN_RESET)//CS SPI
#define MAX7219_CS_Set() 		gpio_set_level(GPIO_NUM_4, GPIO_PIN_SET)


#define MAX7219_REG_DECODE_MODE		0x09
#define MAX7219_REG_INTENSITY			0x0A
#define MAX7219_REG_SCAN_LIMIT		0x0B
#define MAX7219_REG_SHUTDOWN			0x0C
#define MAX7219_REG_DISPLAY_TEST	0x0F




void MAX7219_Test_Self(void);

#endif


