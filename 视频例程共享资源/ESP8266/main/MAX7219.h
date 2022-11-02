#ifndef __MY_MAX7219_H_
#define __MY_MAX7219_H_

#include "mygpio.h"

#define MAX7219_COUNT						4  // chip unmbers
#define MAX7219_LED_ROW_SZ			8  // rows
#define MAX7219_LED_COL_SZ			32 // columns


#define MAX7219_CLK_PIN		GPIO_NUM_12 // D6
#define MAX7219_DIN_PIN		GPIO_NUM_14 // D5
#define MAX7219_CS_PIN		GPIO_NUM_4  // D4

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


void MAX7219_Init(void);
void MAX7219_Shutdown(uint8_t sw);
void MAX7219_Clear(void);

void MAX7219_Write(uint8_t Addr, uint8_t Data);
void MAX7219_Write_Chip(uint8_t ChipID, uint8_t Addr, uint8_t Data);

void MAX7219_Test_Self(void);

#endif


