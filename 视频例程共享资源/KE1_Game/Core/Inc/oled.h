#ifndef _OLED_H_
#define _OLED_H_
#include <stdlib.h>

#include "gpio.h"


#define	Brightness		0xFF
#define X_WIDTH 		128
#define Y_WIDTH 		64	    						  
//-----------------OLED IIC端口定义---------------- 
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);//SCL IIC接口的时钟信号
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);//SDA IIC接口的数据信号
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED接口控制用函数  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_DrawPoint(unsigned char x, unsigned char y, unsigned char w);
//OLED基础函数 
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Set_Pos(unsigned char x, unsigned char y);

void OLED_DrawLogo(void);
void OLED_Show_MyBilibili(void);
void OLED_Draw_BitBirdLogo(void);

void OLED_Show_ChBitBird(void);
void OLED_Show_BirdScore(int score);
void OLED_DrawBird(unsigned char x, unsigned char y);

void OLED_DrawObstacle(unsigned char Page_X, unsigned char Page_End, unsigned char Page_Start);
void OLED_ClearObstacle(unsigned char Page_X, unsigned char Page_End, unsigned char Page_Start);
#endif  

