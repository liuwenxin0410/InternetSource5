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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MOTOR_A_Pin|MOTOR_B_Pin|MOTOR_D_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = MOTOR_A_Pin|MOTOR_B_Pin|MOTOR_D_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = MOTOR_C_Pin|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

void MOTOR_CONTROL(uint8_t direction)
{
	static uint8_t step = 0;
	if(1 == direction){//反转
		if(0 == step) step = 8;
		step--;
	}
	if(0 == step){//步序1
		MOTOR_A_H;
		MOTOR_B_L;
		MOTOR_C_L;
		MOTOR_D_L;
	}else if(1 == step){//步序2
		MOTOR_A_H;
		MOTOR_B_H;
		MOTOR_C_L;
		MOTOR_D_L;
	}else if(2 == step){//步序3
		MOTOR_A_L;
		MOTOR_B_H;
		MOTOR_C_L;
		MOTOR_D_L;
	}else if(3 == step){//步序4
		MOTOR_A_L;
		MOTOR_B_H;
		MOTOR_C_H;
		MOTOR_D_L;
	}else if(4 == step){//步序5
		MOTOR_A_L;
		MOTOR_B_L;
		MOTOR_C_H;
		MOTOR_D_L;
	}else if(5 == step){//步序6
		MOTOR_A_L;
		MOTOR_B_L;
		MOTOR_C_H;
		MOTOR_D_H;
	}else if(6 == step){//步序7
		MOTOR_A_L;
		MOTOR_B_L;
		MOTOR_C_L;
		MOTOR_D_H;
	}else if(7 == step){//步序8
		MOTOR_A_H;
		MOTOR_B_L;
		MOTOR_C_L;
		MOTOR_D_H;
	}
	if(0 == direction){//正转
		step++; if(8 == step) step = 0;
	}
}

void Step_Motor_Start(uint16_t angle, uint8_t direction)
{
	int i = 0;
	int pulse = (int)((double)(angle/5.625)*64);
	for(i=0; i<pulse; i++){
		MOTOR_CONTROL(direction);
		HAL_Delay(2);//延时2ms,修改延时可改变电机转动速度。即修改了脉冲频率
	}
}

void Step_Motor_Stop(void)
{
	//四相输出都为低电平是，电机停止工作
	MOTOR_A_L;
	MOTOR_B_L;
	MOTOR_C_L;
	MOTOR_D_L;
}

//管脚中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_PIN_13 == GPIO_Pin){// 按键1按下发生中断
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);//反转LED等状�??

	}else if(GPIO_PIN_15 == GPIO_Pin){// 按键2按下发生中断
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	}
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
