/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
static uint8_t keyVal = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t row = 0, column = 0;

	if(0 != keyVal) return ;

	if(GPIO_PIN_15 == GPIO_Pin){// row 1
		row = 0x10;
	}else if(GPIO_PIN_3 == GPIO_Pin){// row 2
		row = 0x20;
	}else if(GPIO_PIN_4 == GPIO_Pin){// row 3
		row = 0x30;
	}else if(GPIO_PIN_5 == GPIO_Pin){// row 4
		row = 0x40;
	}

	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){// column 1
		column = 0x01;
	}else if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)){// column 1
		column = 0x02;
	}else if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)){// column 1
		column = 0x03;
	}else if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)){// column 1
		column = 0x04;
	}

	if(0 != row && 0 != column){
		keyVal = row|column;
		printf("key val:%02X\r\n", keyVal);
	}

}

char Get_Key_Ch(void)
{
	char ch = 0;

	if(0 != keyVal){
		if(0x11 == keyVal) ch = '1';
		else if(0x12 == keyVal) ch = '2';
		else if(0x13 == keyVal) ch = '3';
		else if(0x14 == keyVal) ch = 'A';
		else if(0x21 == keyVal) ch = '4';
		else if(0x22 == keyVal) ch = '5';
		else if(0x23 == keyVal) ch = '6';
		else if(0x24 == keyVal) ch = 'B';
		else if(0x31 == keyVal) ch = '7';
		else if(0x32 == keyVal) ch = '8';
		else if(0x33 == keyVal) ch = '9';
		else if(0x34 == keyVal) ch = 'C';
		else if(0x41 == keyVal) ch = '*';
		else if(0x42 == keyVal) ch = '0';
		else if(0x43 == keyVal) ch = '#';
		else if(0x44 == keyVal) ch = 'D';
		keyVal = 0X00;
	}
	return ch;
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
