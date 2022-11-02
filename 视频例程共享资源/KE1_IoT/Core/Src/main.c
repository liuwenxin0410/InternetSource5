/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void vdAscii2hex(char *pcAscii, char *pcHex)
{
	int iDlen = 0, i = 0, pos = 0;
	iDlen = strlen(pcAscii);
	if(128 < iDlen) return;
	for(i=0; i<iDlen; i++){
		sprintf(&pcHex[pos], "%02X", pcAscii[i]);
		pos += 2;
	}
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t aucData[32] = {0};
	uint8_t atCase = 0;
	uint16_t usTimeout = 500;

	float fTemp = 0.0, fHumi = 0.0;
	int dLen = 0, iCnt = 0;
	char acDevInfo[128] = {0}, acHexBuf[256] = {0}, acAtBuf[512] = {0};

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  	printf("Hello NB-IoT Demo !!!!\n");
	UART_Enable_Receive_IT();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	memset(acDevInfo, 0, sizeof(acDevInfo));
	memset(acHexBuf, 0, sizeof(acHexBuf));
	memset(acAtBuf, 0, sizeof(acAtBuf));
	KE1_I2C_SHT31_Init();

	OLED_Init();
	OLED_DrawLogo();

	HAL_Delay(1000);
	OLED_Clear();
	OLED_Show_MyBilibili();

	HAL_Delay(1000);
	OLED_Clear();
	KE1_I2C_SHT31(&fTemp, &fHumi);
	OLED_ShowT_H(fTemp, fHumi);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

#if 1
	  usTimeout = 500;
	  switch(atCase){
		case 0:
			UART_Put_AT_Cmd("AT\r\n");atCase++;
		break;
		case 1:
			UART_Put_AT_Cmd("ATE1\r\n");atCase++;
		break;
		case 2:
			UART_Put_AT_Cmd("AT+CFUN=0\r\n");atCase++;
		break;
		case 3:
			UART_Put_AT_Cmd("AT+CGSN=1\r\n");atCase++;
		break;
		case 4:
			UART_Put_AT_Cmd("AT+NCDP=180.101.147.115,5683\r\n");atCase++;
		break;
		case 5:
			UART_Put_AT_Cmd("AT+CFUN=1\r\n"); usTimeout = 3000;atCase++;
		break;
		case 6:
			UART_Put_AT_Cmd("AT+CGDCONT=1,\"IP\",\"CTNB\"\r\n");atCase++;
		break;
		case 7:
			UART_Put_AT_Cmd("AT+CGATT=1\r\n");atCase++;
		break;
		case 8:
			//UART_Put_AT_Cmd("AT+CGATT?\r\n");atCase++;
		break;
		case 9:
			//UART_Put_AT_Cmd("AT+CSQ\r\n");
		break;
		case 10:
			KE1_I2C_SHT31(&fTemp, &fHumi);
			OLED_ShowT_H(fTemp, fHumi);
//				  printf("T:%0.2f,H:%0.2f\r\n", fTemp, fHumi);
			memset(acDevInfo, 0, sizeof(acDevInfo));
			memset(acHexBuf, 0, sizeof(acHexBuf));
			memset(acAtBuf, 0, sizeof(acAtBuf));

			dLen = snprintf(acDevInfo, sizeof(acDevInfo), "{\"T\":\"%0.2f\",\"H\":\"%0.2f\"}", fTemp, fHumi);// 打包设备传感器数�?
			vdAscii2hex(acDevInfo, acHexBuf);
			snprintf(acAtBuf, sizeof(acAtBuf), "AT+NMGS=%d,00%04X%s\r\n", (dLen+3), dLen, acHexBuf);
			printf("%s\r\n", acDevInfo);
			UART_Put_AT_Cmd(acAtBuf);

//			UART_Put_AT_Cmd("AT+NMGS=69,0000427B2254223A2232332E3132222C2248223A2236342E3733222C224C223A223231222C2253223A2232313037222C2256223A2234313430222C224E42223A223234227D\r\n");
			atCase++;
			iCnt = 0;
		break;
	  }
	  dLen = UART_Get_AT_Resp(aucData, sizeof(aucData), usTimeout);
	  if(0 < dLen){
		  printf("--- AT RESP ---\r\n");
		  printf("%s", aucData);

		  if(NULL != strstr((char *)aucData, "+QLWEVTIND:3")){
			  atCase = 10;
		  }
		  memset(aucData, 0, sizeof(aucData));
	  }

#endif
	  HAL_Delay(1000);
	  iCnt++;
	  if(3600 == iCnt){
		  atCase = 10;
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
