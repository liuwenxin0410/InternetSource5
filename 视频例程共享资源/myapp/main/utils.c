#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"

#define hex(x)		((x)<=9 ? ((x)+'0') : ((x)+('A'-0x0a)) )
#define odd(x)		((x) & 0x01)
#define even(x)		(!odd(x))

static unsigned char	char_ascii_to_hex(unsigned char ascii)
{
    unsigned char	val;

    val = ascii;
    if( (val>='0') && (val<='9')){
      val = val & 0x0f;
    }
    else if( (val>='a') && (val<='f')){
      val = val - 'a' + 0x0a;
    }
    else if( (val>='A') && (val<='F')){
      val = val - 'A' + 0x0a;
    }else{
      val = 0;
    }
    return( val);
}

void AscToHex(unsigned char *hex_buf, const char *ascii_buf,int ascii_len)
{
  int	i, len;

  len = (ascii_len+1) / 2;
  for( i=0; i<len; i++){
    *hex_buf  = char_ascii_to_hex( *ascii_buf) << 4;	/* high nibble */
    ascii_buf++;
    *hex_buf |= char_ascii_to_hex( *ascii_buf);		/* low  nibble */
    ascii_buf++;
    hex_buf++;
  }
  if( odd(ascii_len) ){
    hex_buf--;
    *hex_buf &= 0xf0;	/* odd lengths have the extra digit set to 0 */
  }
}

char hexStrBuf[128] = {0};
void print_hex(const char *pTag, const unsigned char *pData, int iLen)
{
  int i = 0;
  char acTmp[3] = {0};
  memset(hexStrBuf, 0, sizeof(hexStrBuf));

  for(i=0; i<iLen; i++){
    sprintf(acTmp, "%02X", pData[i]);
    strcat(hexStrBuf, acTmp);
    if(strlen(hexStrBuf)+10 > sizeof(hexStrBuf)){
      strcat(hexStrBuf, " ...");
      break;
    }
  }
	ESP_LOGI(pTag, "%s", hexStrBuf);
}



