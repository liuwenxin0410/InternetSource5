#include <stdio.h>
#include <string.h>

#include "oled.h"
#include "bilibili_logo.h"
#include "bit_bird_bmp.h"
#include "oledfont.h"  	

//OLED的显存
//存放格式如下.
//-----------------------------------
//|x→[0,127]						|
//|			OLED显示坐标			|
//|y			范围				|
//|↓								|
//|[0,64]							|
//-----------------------------------			   
/**********************************************
//IIC Start
**********************************************/
void IIC_Start(void)
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop(void)
{
	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SCLK_Set();
	OLED_SDIN_Set();
}

/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	for(i=0;i<8;i++){
		if(IIC_Byte & 0x80){
			OLED_SDIN_Set();
		}else{
			OLED_SDIN_Clr();
		}
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		IIC_Byte<<=1;
	}
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);     //Slave address,SA0=0
	Write_IIC_Byte(0x00);			//write command
	Write_IIC_Byte(IIC_Command); 
	IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	Write_IIC_Byte(0x40);			//write data
	Write_IIC_Byte(IIC_Data);
	IIC_Stop();
}
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd){
		Write_IIC_Data(dat);
	}
	else {
		Write_IIC_Command(dat);	
	}
}

/********************************************
// fill_Picture
********************************************/
void OLED_Fill(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++){
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<X_WIDTH;n++){
			OLED_WR_Byte(fill_Data,1);
		}
	}
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//注意：清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++){
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<X_WIDTH;n++)
			OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++){
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}

//初始化OLED
void OLED_Init(void)
{
	HAL_Delay(500);

	OLED_WR_Byte(0xae, OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00, OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10, OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40, OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81, OLED_CMD);//--set contrast control register
	OLED_WR_Byte(Brightness, OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xa1, OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xc8, OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xa6, OLED_CMD);//--set normal display
	OLED_WR_Byte(0xa8, OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f, OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xd3, OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5, OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80, OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xd9, OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xf1, OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xda, OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0xdb, OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40, OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20, OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD);//
	OLED_WR_Byte(0x8d, OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xa4, OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xa6, OLED_CMD);// Disable Inverse Display On (0xa6/a7)
	OLED_WR_Byte(0xaf, OLED_CMD);//--turn on oled panel
	OLED_Clear();
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~7 page
//
//size:选择字体 16/12
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{
	unsigned char c=0,i=0;
	c=chr-' ';//得到偏移后的值
	if(x > X_WIDTH-1){x=0;y=y+2;}
	if(Char_Size == 16){
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++){
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);}
			OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++){
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);}
	}else {
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA);
	}
}

//显示一个字符号串
// x:0~127  y: 0~7
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr, uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0'){
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120){
			x=0;y+=2;
		}
		j++;
	}
}
//显示汉字
// x:0~127  y: 0~7
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
	for(t=0;t<16;t++){
		OLED_WR_Byte(F_Chx16[2*no][t],OLED_DATA);
		adder+=1;
	}	
	OLED_Set_Pos(x,y+1);	
	for(t=0;t<16;t++){
		OLED_WR_Byte(F_Chx16[2*no+1][t],OLED_DATA);
		adder+=1;
	}
}

// x:0~128  y: 0~664
void OLED_DrawPoint(unsigned char x, unsigned char y, unsigned char w)
{
	/*
	 * page 从低位开始
	 * */
	unsigned char page = y/8;
	unsigned char pos = y%8;
	unsigned char point = 0x0F, pBase = 0x0F,pW = 2, pH = 4;
	//14, 26, 50
	if(14 >= w) {point = 0x03; pBase = 0x03; pW = 2, pH = 2;}
	else if(14 < w && 26 >= w) {point = 0x0F; pBase = 0x0F; pW = 4, pH = 4;}
	else if(26 < w && 50 >= w) {point = 0x3F; pBase = 0x3F; pW = 6, pH = 6;}
	else if(50 < w) {point = 0xFF; pBase = 0xFF; pW = 8, pH = 8;}

	static uint8_t _x = 0, _page = 0, _pW = 0;

	OLED_Set_Pos(_x, _page); // 清除上一个点
	do{
		OLED_WR_Byte(0, OLED_DATA);
	}while(_pW--);

	if(pH < pos) {
		pos = 8 - pos;
		point = (pBase<<pos);
	}
	if(8 == page) page = 7;

	//printf("%d-%d-%02X\r\n", page, pos, point);

	_x = x; _page = page; _pW = pW;

	OLED_Set_Pos(x, page);
	do{
		OLED_WR_Byte(point, OLED_DATA);
	}while(pW--);

}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8 == 0) y = y1/8;
	else y = y1/8+1;
	for(y=y0; y<y1; y++){
		OLED_Set_Pos(x0,y);
		for(x=x0; x<x1; x++){
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
} 

//x: 0~127  y:0~56
void OLED_DrawBird(unsigned char x, unsigned char y)
{
	/*
	 * page 从低位开始
	 * */
	unsigned char page1 = y/8, page2 = 0;
	unsigned char pos = y%8;

	unsigned char p = 0;
	unsigned char aucBird[8] = {0x18,0x2C,0x2C,0xE7,0x81,0x66,0x24,0x3C};
	static unsigned char aucBird_u[8] = {0};
	static unsigned char aucBird_d[8] = {0};
	static unsigned char _x = 0, _page1 = 0, _page2 = 0;

	if(127 < x || 56 < y) return;

	if(0 != pos){
		page2 = page1+1;

		for(p=0; p<8; p++){
			aucBird_u[p] = aucBird[p]<<pos;
			aucBird_d[p] = aucBird[p]>>(8-pos);
		}
	}else{
		for(p=0; p<8; p++){
			aucBird_u[p] = aucBird[p];
		}
	}


	OLED_Set_Pos(_x, _page1);
	for(p=0; p<8; p++){
		OLED_WR_Byte(0, OLED_DATA);
	}
	if(_page2){
		OLED_Set_Pos(_x, _page2);
		for(p=0; p<8; p++){
			OLED_WR_Byte(0, OLED_DATA);
		}
	}

	//printf("%d-%d-%02X\r\n", page, pos, point);

	_x = x; _page1 = page1; _page2 = page2;

	OLED_Set_Pos(x, page1);
	for(p=0; p<8; p++){
		OLED_WR_Byte(aucBird_u[p], OLED_DATA);
	}
	if(page2){
		OLED_Set_Pos(x, page2);
		for(p=0; p<8; p++){
			OLED_WR_Byte(aucBird_d[p], OLED_DATA);
		}
	}
}

void OLED_DrawObstacle(unsigned char Page_X, unsigned char Page_End, unsigned char Page_Start)
{
	unsigned char point = 0xFF, pW = 4;
	unsigned char a = 0, b = 0;

	if(4 > Page_X) pW = Page_X;
	else if(124 <= Page_X) pW = 127-Page_X;

	for(a=0; a<Page_End; a++){
		OLED_Set_Pos(Page_X, a);
		for(b=0; b<pW; b++){
			OLED_WR_Byte(point, OLED_DATA);
		}
	}

	for(a=Page_Start; a<8; a++){
		OLED_Set_Pos(Page_X, a);
		for(b=0; b<pW; b++){
			OLED_WR_Byte(point, OLED_DATA);
		}
	}
}

void OLED_ClearObstacle(unsigned char Page_X, unsigned char Page_End, unsigned char Page_Start)
{
	unsigned char pW = 4;
	unsigned char a = 0, b = 0;

	if(4 > Page_X) pW = Page_X;
	else if(124 <= Page_X) pW = 127-Page_X;

	/* clear old  */
	for(a=0; a<Page_End; a++){
		OLED_Set_Pos(Page_X, a);
		for(b=0; b<pW; b++){
			OLED_WR_Byte(0, OLED_DATA);
		}
	}

	for(a=Page_Start; a<8; a++){
		OLED_Set_Pos(Page_X, a);
		for(b=0; b<pW; b++){
			OLED_WR_Byte(0, OLED_DATA);
		}
	}
}

void OLED_DrawLogo(void)
{
	OLED_DrawBMP(15, 0, 113, 4, BILIBILI_LOGO_BMP);
}

void OLED_Draw_BitBirdLogo(void)
{
	uint8_t x = 32, y = 5, i = 3;

	OLED_DrawBMP(38, 0, 78, 4, BIT_BIRD_BMP);
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1);
}

void OLED_Show_MyBilibili(void)
{
	uint8_t x = 32, y = 5, i = 1;

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1);
	OLED_ShowString(x+1, y, (uint8_t *)"IoT", 16);
	//OLED_ShowString(0, 2, (uint8_t *)"bilibili", 16);
}

void OLED_Show_BitBird(char x, char y)
{
	unsigned char aucBird[8] = {0x18,0x2C,0x2C,0xE7,0x81,0x66,0x24,0x3C};
	uint8_t p = 0;

	OLED_Set_Pos(x, y);
	for(p=0; p<8; p++){
		OLED_WR_Byte(aucBird[p], OLED_DATA);
	}
}

void OLED_Show_ChBitBird(void)
{
	uint8_t x = 40, y = 0, i = 3;

	OLED_Show_BitBird(30, 1);
	OLED_Show_BitBird(91, 1);

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1);
}

void OLED_Show_BirdScore(int score)
{

	uint8_t acShowCnt[10] = {0};


	OLED_Clear();

	OLED_ShowString(24, 1, (uint8_t*)"Game Over!", 16);

	OLED_Show_BitBird(24, 5);
	snprintf((char *)acShowCnt, sizeof(acShowCnt)-1, ":%d", score);
	OLED_ShowString(33, 4, acShowCnt, 16);
}

