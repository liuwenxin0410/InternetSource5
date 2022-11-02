/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 32-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA5     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA5     ------> TIM2_CH1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#include <stdio.h>
#include "oled.h"

float   dis_fm = 0;		     //测量距离
uint8_t   Edge = 0;			//上升沿下降沿捕获标志
uint16_t  HighTime; 		//高电平时间
uint16_t  RisingTime;		//捕获上升沿时间点
uint16_t  FallingTime;	    //捕获下降沿时间点

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//中断回调函数
{
  /*防止未使用参数编译警告*/
	UNUSED(htim);

	if(htim->Instance == htim2.Instance)  //TIMER2
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  //CHANNEL_1
		{
			if(Edge == 0)          //捕获上升沿
			{
				RisingTime = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);    //获取上升沿时间点
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);   //切换捕获极性为下降沿
				HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);     //重启中断

				Edge = 1;          	 //捕获极性标值换为下降沿
			}
			else if(Edge == 1)     //捕获下降沿
			{
				FallingTime = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);    //获取下降沿时间点
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);     //切换捕获极性为上升沿
				HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);   	 //重启中断

				HighTime = FallingTime < RisingTime ? FallingTime + 0xffff - RisingTime + 1 : FallingTime - RisingTime;
				//高电平时�?=下降沿时�? - 上升沿时�?(若下降沿时间<上升沿时间FallingTime + 0xffff - RisingTime + 1)

				dis_fm = (float)((HighTime*340)/10000)/2; //距离计算 (HighTime*(1/1000000)秒)   *34400cm/2
				printf("Distance =  %d - %.1f cm\r\n", HighTime, dis_fm);
				OLED_Show_Distance(dis_fm);
				Edge = 0;		  			 //1次采集完毕，复位捕获上升沿标记
			}
		}
	}
}

void Delay_us(uint32_t t)//延时微秒
{
    int n  =  11;
    while(--t)
    {
        while(--n);
        n = 11;
    }
}

void Ultrasonic_Trig_Start(void)//Trig触发超声波发送
{

  HAL_GPIO_WritePin(GPIOA , GPIO_PIN_7 , GPIO_PIN_SET);

  Delay_us(20);//延时20微秒

  HAL_GPIO_WritePin(GPIOA , GPIO_PIN_7, GPIO_PIN_RESET);
}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
