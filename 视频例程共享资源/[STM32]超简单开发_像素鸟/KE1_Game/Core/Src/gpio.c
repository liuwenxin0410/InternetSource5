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

#include  "oled.h"
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
uint8_t keyConfirm = 0, keyJump = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(1 == keyConfirm || 1 == keyJump) return;

	if(GPIO_PIN_15 == GPIO_Pin){
		keyJump = 1;
	}else if(GPIO_PIN_13 == GPIO_Pin){
		keyConfirm = 1;
	}
	printf("g\r\n");
}

uint8_t Get_KeyConfirm(void)
{
	if(0 == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
		HAL_Delay(100);
		if(1 == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
			return 1;
		}
	}
	return 0;
}

uint8_t Get_KeySetting(void)
{
	if(0 == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)){
		HAL_Delay(100);
		if(1 == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)){
			return 1;
		}
	}
	return 0;
}

uint8_t Get_KeyJump(void)
{
	if(0 == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)){
		return 1;
	}
	return 0;
}

uint8_t Get_KeyDown(void)
{
	if(0 == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
		return 1;
	}
	return 0;
}

uint8_t tX = 112, tY = 32, tLevel = 1;
void Bird_Ready(uint8_t x, uint8_t y)
{
	OLED_DrawBird(x, y);
	tX = x, tY = y;
}

void Move_Bird(void)
{
	uint8_t jump = 0;
	static uint16_t timCnt = 0;
	HAL_Delay(30);

	if(1 == Get_KeyJump()){
		jump = 1;
		if(0 != tY) tY--;
	}else if(1 == Get_KeyDown()){
		jump = 2;
		if(56 != tY) tY++;
	}else{
		timCnt++;
		if(5 == timCnt){
			timCnt = 0;
			if(56 != tY) {jump = 3; tY++;}
		}
	}

	if(0 != jump){
		printf("%d-%d\r\n", tX, tY);
		timCnt = 0;
		OLED_DrawBird(tX, tY);
	}
}

void Select_Level(char flag)
{
	uint8_t setting = 0;
	uint8_t showLevel[10] = {0};

	setting = Get_KeySetting();

	if(1 == setting){
		if(5 != tLevel) tLevel++;
		else tLevel = 1;
	}

	if(0 == flag){
		snprintf((char *)showLevel, sizeof(showLevel)-1, "LEVEL:%d", tLevel);
		OLED_ShowString(34, 3, showLevel, 16);
	}else{
		if(0 != setting){
			snprintf((char *)showLevel, sizeof(showLevel)-1, "LEVEL:%d", tLevel);
			OLED_ShowString(34, 3, showLevel, 16);
		}
	}
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
