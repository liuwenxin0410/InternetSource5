
#include "esp_log.h"
#include "esp_system.h"

#include "max7219.h"

#include "font/bit_led_font.h"

void BITLED_Clock_Show_Hour(uint8_t hour)
{
	uint8_t chipId = 0, i = 0, j = 0;
	uint8_t leftNum = 0, rightNum = 0, secData[7] = {0};
	
	leftNum = hour/10;
	rightNum = hour%10;

	for(i=0; i<7; i++){
		secData[i] = (BIT_LED_Num_F3x7[leftNum][i]<<5)|(BIT_LED_Num_F3x7[rightNum][i]);
	}
	
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}
}

void BITLED_Clock_Show_Min(uint8_t min)
{
	uint8_t chipId = 1, i = 0, j = 0;
	uint8_t leftNum = 0, rightNum = 0, secData[7] = {0};
	
	leftNum = min/10;
	rightNum = min%10;

	for(i=0; i<7; i++){
		if(2 == i || 4 == i){
			secData[i] = 0x40|(BIT_LED_Num_F3x7[leftNum][i]<<1);
		}else{
			secData[i] = BIT_LED_Num_F3x7[leftNum][i]<<1;
		}
	}
	
	chipId = 1;
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}

	for(i=0; i<7; i++){
		if(2 == i || 4 == i){
			secData[i] = (BIT_LED_Num_F3x7[rightNum][i]<<4)|0x04;
		}else{
			secData[i] = BIT_LED_Num_F3x7[rightNum][i]<<4;
		}
	}

	chipId = 2;
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}
}

void BITLED_Clock_Show_Sec(uint8_t sec)
{
	uint8_t chipId = 3, i = 0, j = 0;
	uint8_t leftNum = 0, rightNum = 0, secData[7] = {0};
	
	leftNum = sec/10;
	rightNum = sec%10;

	for(i=0; i<7; i++){
		secData[i] = (BIT_LED_Num_F3x7[leftNum][i]<<5)|(BIT_LED_Num_F3x7[rightNum][i]);
	}
	
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}
}

void BITLED_Show_DHT11(uint8_t dataBuf[5])
{
	uint8_t chipId = 0, i = 0, j = 0;
	
	uint8_t tempH = 0, tempL = 0;
	uint8_t humiH = 0, humiL = 0;

	uint8_t leftNum = 0, rightNum = 0, secData[7] = {0};
	
	humiH = dataBuf[0]; humiL = dataBuf[1];
	tempH = dataBuf[2]; tempL = dataBuf[3];
	if(100 <= humiH || 10 <= humiL) return;
	if(50 < tempH || 10 <= tempL) return;
	/** show temperature **/
	leftNum = tempH/10;
	rightNum = tempH%10;
	
	for(i=0; i<7; i++){
		secData[i] = (BIT_LED_Num_F3x7[leftNum][i]<<5)|BIT_LED_Num_F3x7[rightNum][i];
	}

	chipId = 0;
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}

	for(i=0; i<7; i++){
		if(6 == i){
			secData[i] = 0x40|(BIT_LED_Num_F3x7[tempL][i]<<2);
		}else{
			secData[i] =  BIT_LED_Num_F3x7[tempL][i]<<2;
		}
	}

	chipId = 1;
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}
	
	/** show humidity **/
	leftNum = humiH/10;
	rightNum = humiH%10;
	
	for(i=0; i<7; i++){
		secData[i] = BIT_LED_Num_F3x7[leftNum][i]<<3|(BIT_LED_Num_F3x7[rightNum][i]>>2);
	}

	chipId = 2;
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}

	for(i=0; i<7; i++){
		if(6 == i){
			secData[i] = ((BIT_LED_Num_F3x7[rightNum][i]<<6)|0x10)|BIT_LED_Num_F3x7[humiL][i];
		}else{
			secData[i] = (BIT_LED_Num_F3x7[rightNum][i]<<6)|BIT_LED_Num_F3x7[humiL][i];
		}
	}

	chipId = 3;
	MAX7219_Write_Chip(chipId, 1, 0);
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(chipId, j, secData[i]);
	}
}

void BITLED_Show_Logo(void)
{
	uint8_t i = 0, j = 0;
	uint8_t ledData[][7] = {{0xE0, 0x88, 0x80, 0xEB, 0x88, 0x88, 0x88}, // Fi-
													 {0x38, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3A}, // Cl
													 {0x00, 0x63, 0x94, 0x94, 0x94, 0x94, 0x63}, // oc
													 {0x00, 0x40, 0x50, 0x60, 0x40, 0x60, 0x50}};// k0
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(0, j, ledData[0][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(1, j, ledData[1][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(2, j, ledData[2][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(3, j, ledData[3][i]);
	}
}

void BITLED_Show_ConnectWiFi(void)
{
	uint8_t i = 0, j = 0;
	uint8_t ledData[][7] = {{0x0E, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0E}, // P
													 {0x11, 0x15, 0x15, 0xD5, 0x15, 0x15, 0x0E}, // -W
													 {0x5D, 0x10, 0x51, 0x5D, 0x51, 0x51, 0x51}};// iFi
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(0, j, ledData[0][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(1, j, ledData[1][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(2, j, ledData[2][i]);
	}
}


void BITLED_Show_PairWiFi(void)
{
	uint8_t i = 0, j = 0;
	uint8_t ledData[][7] = {{0x1E, 0x12, 0x12, 0x1C, 0x10, 0x10, 0x10}, // P
													 {0x11, 0x15, 0x15, 0xD5, 0x15, 0x15, 0x0E}, // -W
													 {0x5D, 0x10, 0x51, 0x5D, 0x51, 0x51, 0x51}};// iFi
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(0, j, ledData[0][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(1, j, ledData[1][i]);
	}
	for(i=0, j=2; i<7; i++, j++){
		MAX7219_Write_Chip(2, j, ledData[2][i]);
	}
}

void BITLED_Show_Doing_Action(uint8_t clearFlag)
{
	static uint8_t pos = 0, direction = 0;
	static uint8_t ledData[1] = {0};
	uint8_t ledBit = 0, ledChip = 3, ledPos = 0;

	if(1 == clearFlag) {
		pos = 0; memset(ledData, 0, sizeof(ledData)); return;
	}
	
	ledPos = pos%8;

	ledBit = ledData[0];
	if(0 == direction){
		ledData[0] = ledBit|(0x80>>ledPos);
	}else{
		ledData[0] = ledBit&((0x80>>ledPos)^0xFF);
	}
	MAX7219_Write_Chip(ledChip, 8, ledData[0]);

	pos++; 
	if(8 == pos) {
		if(0 == direction) direction = 1;
		else direction = 0;
		
		pos = 0;
	}
}

void BITLED_Show_SNTP_Action(uint8_t clearFlag)
{
	static uint8_t pos = 0, direction = 0;
	static uint8_t ledData[MAX7219_COUNT] = {0};
	uint8_t ledBit = 0, ledChip = 0, ledPos = 0;

	if(1 == clearFlag) {
		pos = 0; memset(ledData, 0, sizeof(ledData)); return;
	}
	
	ledChip = pos/8;
	ledPos = pos%8;

	ledBit = ledData[ledChip];
	if(0 == direction){
		ledData[ledChip] = ledBit|(0x80>>ledPos);
	}else{
		ledData[ledChip] = ledBit&((0x80>>ledPos)^0xFF);
	}
	MAX7219_Write_Chip(ledChip, 4, ledData[ledChip]);

	pos++; 
	if(MAX7219_LED_COL_SZ == pos) {
		if(0 == direction) direction = 1;
		else direction = 0;
		
		pos = 0;
	}
}

