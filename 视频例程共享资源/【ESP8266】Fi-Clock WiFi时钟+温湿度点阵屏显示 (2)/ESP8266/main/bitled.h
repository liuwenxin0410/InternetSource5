#ifndef __MY_BIT_LED_H_
#define __MY_BIT_LED_H_

#include "max7219.h"

void BITLED_Clock_Show_Hour(uint8_t hour);
void BITLED_Clock_Show_Min(uint8_t min);
void BITLED_Clock_Show_Sec(uint8_t sec);
void BITLED_Show_DHT11(uint8_t dataBuf[5]);

void BITLED_Show_Logo(void);
void BITLED_Show_ConnectWiFi(void);
void BITLED_Show_PairWiFi(void);
void BITLED_Show_Doing_Action(uint8_t clearFlag);
void BITLED_Show_SNTP_Action(uint8_t clearFlag);
#endif


