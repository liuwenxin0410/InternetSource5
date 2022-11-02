#ifndef _OLED_H_
#define _OLED_H_
#include <stdlib.h>

#include "stm32l4xx_hal.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL			0x00
#define XLevelH			0x10
#define Max_Column	128
#define Max_Row			64
#define	Brightness	0xFF 
#define X_WIDTH 		128
#define Y_WIDTH 		64	    						  
//-----------------OLED IIC端口定义---------------- 
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_RESET);//SCL IIC接口的时钟信号
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_SET);

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);//SDA IIC接口的数据信号
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED接口控制用函数  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);



//OLED基础函数 
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Set_Pos(unsigned char x, unsigned char y);

void OLED_DrawLogo(void);
void OLED_ShowKE1(void);
void OLED_ShowT_H(float T, float H);
void OLED_ShowVoltage(uint16_t V);
void OLED_ShowS_L(uint16_t S, uint16_t L);
void OLED_ShowInRegister(int cnt);
void OLED_ShowRegisterOK(void);
void OLED_Show_UP_Flag(char cFlag);
void OLED_Show_Note(const char *pcMsg, int type, int val);

void OLED_Show_MyBilibili(void);
#endif  

