#include <stdio.h>
#include <string.h>

#include "oled.h"
#include "bilibili_logo.h"
#include "oledfont.h"  	

//OLED的显存
//存放格式如下.
//-----------------------------------
//|x→[0,127]						|
//|			OLED显示坐标			|
//|y			范围				|
//|↓								|
//|[0,31]							|
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
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

void IIC_Wait_Ack(void)
{
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		m=m&0x80;
		if(m==0x80)
		{
			OLED_SDIN_Set();
		}
		else 
			OLED_SDIN_Clr();
		da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);     //Slave address,SA0=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
	IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
	IIC_Stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		Write_IIC_Data(dat);
	}
	else 
	{
		Write_IIC_Command(dat);	
	}
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
		{
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
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}
void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>Max_Column-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++){
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);}
			OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++){
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);}
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA);
	}
}
//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else 
				enshow=1; 
		}
		OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr, uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120)
		{
			x=0;y+=2;
		}
		j++;
	}
}
//显示汉字
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
	for(t=0;t<16;t++)
	{
		OLED_WR_Byte(F_Chx16[2*no][t],OLED_DATA);
		adder+=1;
	}	
	OLED_Set_Pos(x,y+1);	
	for(t=0;t<16;t++)
	{	
		OLED_WR_Byte(F_Chx16[2*no+1][t],OLED_DATA);
		adder+=1;
	}
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
} 

//初始化OLED			    
void OLED_Init(void)
{ 	
	HAL_Delay(200);
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭显示

	OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
	OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address

	OLED_WR_Byte(0xC8,OLED_CMD);//-not offset

	OLED_WR_Byte(0x81,OLED_CMD);//设置对比度
	OLED_WR_Byte(0xff,OLED_CMD);

	OLED_WR_Byte(0xa1,OLED_CMD);//段重定向设置

	OLED_WR_Byte(0xa6,OLED_CMD);//

	OLED_WR_Byte(0xa8,OLED_CMD);//设置驱动路数
	OLED_WR_Byte(0x1f,OLED_CMD);

	OLED_WR_Byte(0xd3,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);

	OLED_WR_Byte(0xd5,OLED_CMD);
	OLED_WR_Byte(0xf0,OLED_CMD);

	OLED_WR_Byte(0xd9,OLED_CMD);
	OLED_WR_Byte(0x22,OLED_CMD);

	OLED_WR_Byte(0xda,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);

	OLED_WR_Byte(0xdb,OLED_CMD);
	OLED_WR_Byte(0x49,OLED_CMD);

	OLED_WR_Byte(0x8d,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);

	OLED_WR_Byte(0xaf,OLED_CMD);
	OLED_Clear();
}  

void OLED_DrawLogo(void)
{
	OLED_DrawBMP(0,0,98, 7, BILIBILI_LOGO_BMP);
}

void OLED_ShowKE1(void)
{
	uint8_t x = 0, y = 0, i = 1;
	OLED_Clear();
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	
	OLED_ShowString(x, y, (uint8_t *)"KE1", 16);
}

void OLED_ShowT_H(float T, float H)
{
	char acOledLineMsg[17] = {0};
	uint8_t x = 0, y = 0, i = 7;
	int len;
	
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1);
	
	len = snprintf(acOledLineMsg, sizeof(acOledLineMsg), "%.2f'C %.2f%%", T, H);
	memset(acOledLineMsg+len, 0x20, sizeof(acOledLineMsg)-len-1);
	OLED_ShowString(0, 2, (uint8_t *)acOledLineMsg, 16);
}

void OLED_ShowVoltage(uint16_t V)
{
	char acOledLineMsg[17] = {0};
	uint8_t x = 0, y = 0, i = 10;
	int len;
	
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); 
	
	len = snprintf(acOledLineMsg, sizeof(acOledLineMsg), "%dmV", V);
	memset(acOledLineMsg+len, 0x20, sizeof(acOledLineMsg)-len-1);
	OLED_ShowString(0, 2, (uint8_t *)acOledLineMsg, 16);
}

void OLED_ShowS_L(uint16_t S, uint16_t L)
{
	char acOledLineMsg[17] = {0};
	uint8_t x = 0, y = 0, i = 14;
	int len;
	
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowString(x, y, (uint8_t *)"/", 16);x += (8+1);
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); 
	
	len = snprintf(acOledLineMsg, sizeof(acOledLineMsg), "AD:%d/%d", S, L);
	memset(acOledLineMsg+len, 0x20, sizeof(acOledLineMsg)-len-1);
	OLED_ShowString(0, 2, (uint8_t *)acOledLineMsg, 16);
}

void OLED_ShowInRegister(int cnt)
{
	char acOledLineMsg[4] = {0};
	uint8_t x = 0, y = 0, i = 18;

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;

	snprintf(acOledLineMsg, sizeof(acOledLineMsg), "%03d", cnt);
	OLED_ShowString(x, 1, (uint8_t *)acOledLineMsg, 8);
}

void OLED_ShowRegisterOK(void)
{
	uint8_t x = 0, y = 0, i = 18;

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;

	i = 23;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1); i++;

	i = 0;
	OLED_ShowCHinese(x, y, i); x += (16+1);
}

void OLED_Show_UP_Flag(char cFlag)
{
	if(0 == cFlag){
		OLED_ShowString(105, 0, (uint8_t *)"  ", 8);
	}else if(1 == cFlag){
		OLED_ShowString(105, 0, (uint8_t *)"->", 8);
	}else if(2 == cFlag){
		OLED_ShowString(105, 0, (uint8_t *)"-C", 8);
	}
}

void OLED_Show_Note(const char *pcMsg, int type, int val)
{
	char acLCDShow[8] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0};
	char acTemp[8] = {0}, len = 0;
	if(1 == type){
		len = snprintf(acTemp, sizeof(acTemp),"Reg:%d", val);
	}else if(2 == type){
		len = snprintf(acTemp, sizeof(acTemp),"NB:%d", val);
	}else if(0 != pcMsg){
		len = snprintf(acTemp, sizeof(acTemp),"%s", pcMsg);
	}
	memcpy(acLCDShow, acTemp, len);
	OLED_ShowString(60, 0, (uint8_t *)acLCDShow, 6);
}

void OLED_Show_MyBilibili(void)
{
	uint8_t x = 0, y = 1, i = 25;

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1);
	OLED_ShowString(x+1, y, (uint8_t *)"IoT", 16);
	//OLED_ShowString(0, 2, (uint8_t *)"bilibili", 16);
}

void OLED_Show_Distance(float dm)
{
	char acLCDShow[10] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0};
	char acTemp[10] = {0};
	uint8_t x = 0, y = 1, i = 27, len = 0;

	len = snprintf(acTemp, sizeof(acTemp), ":%.1fcm", dm);
	memcpy(acLCDShow, acTemp, len);

	OLED_ShowCHinese(x, y, i); x += (16+1); i++;
	OLED_ShowCHinese(x, y, i); x += (16+1);
	OLED_ShowString(x+1, y, (uint8_t *)acLCDShow, 16);
}
