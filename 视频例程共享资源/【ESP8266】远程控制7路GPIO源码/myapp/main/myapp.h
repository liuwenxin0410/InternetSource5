
#ifndef _MY_APP_H_
#define _MY_APP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define APP_NAME 			"MQIoT Bilibili"
#define APP_VERSION 	"0.0.6"

typedef enum{
	SYS_OK = 1,
	SYS_DOING_WIFI_CONNECT,
	SYS_DOING_SMARTCFG,
	SYS_DOING_CONNECTION
}SYS_LED_STA;


typedef enum{
	CTRL_GPIO = 0xD1
	
}SUPPORT_CMD;


#endif

