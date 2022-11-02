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
#include <stdio.h>
#include <stdlib.h>
#include "oled.h"
/* USER CODE END 0 */

TIM_HandleTypeDef htim1;

/* TIM1 init function */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 80-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* TIM1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#define OBSTACLE_SZ		4
static uint8_t AllObstacle[OBSTACLE_SZ][8] = {{0,0,3,5,1,0,0,0}, {0,0,2,4,0,0,0,0}, {0,0,5,7,0,0,0,0}, {0,0,1,3,0,0,0,0}};
static uint8_t crashFlag = 0;
static int userScore = 0;
extern uint8_t tX, tY, tLevel;

void Check_iF_Crash(uint8_t oX, uint8_t oS, uint8_t oE);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t tim1Cnt = 0;
	uint16_t levelCnt = 100;

	uint8_t a = 0;
	uint8_t tag_X = 0, _tag_X = 0, page_S = 0, page_E = 0, tag_Show = 0;

	if(1 == tLevel){
		levelCnt = 150;
	}else if(2 == tLevel){
		levelCnt = 100;
	}else if(3 == tLevel){
		levelCnt = 80;
	}else if(4 == tLevel){
		levelCnt = 60;
	}else if(5 == tLevel){
		levelCnt = 30;
	}

	if(TIM1 == htim->Instance){
		tim1Cnt++;
		if(levelCnt == tim1Cnt) {
			tim1Cnt = 0;

			for(a=0; a<OBSTACLE_SZ; a++){
				tag_X = AllObstacle[a][0];
				_tag_X = AllObstacle[a][1];
				page_S = AllObstacle[a][2];
				page_E = AllObstacle[a][3];
				tag_Show = AllObstacle[a][4];

				Check_iF_Crash(tag_X, page_S, page_E);

				if(1 == tag_Show){
					OLED_ClearObstacle(_tag_X, page_S, page_E);
					OLED_DrawObstacle(tag_X, page_S, page_E);
					_tag_X = tag_X;
					tag_X++;
					if(128 == tag_X) {
						tag_X = 0; tag_Show = 0;

						page_S = rand()%7;
						if(AllObstacle[a][2] == page_S) page_S = 7-page_S;

						if(5 < page_S) page_S = 5;


						page_E = page_S+2;

						AllObstacle[a][2] = page_S; AllObstacle[a][3] = page_E;
					}
					AllObstacle[a][0] = tag_X;
					AllObstacle[a][1] = _tag_X;
					AllObstacle[a][4] = tag_Show;
				}

				if(37 == tag_X){
					if(OBSTACLE_SZ-1 != a) AllObstacle[a+1][4] = 1;
					else if(OBSTACLE_SZ-1 == a) AllObstacle[0][4] = 1;
				}
			}
		}
	}

}

void TIM1_IT_Start(void)
{
	HAL_TIM_Base_Start_IT(&htim1);
}

void TIM1_IT_Stop(void)
{
	HAL_TIM_Base_Stop_IT(&htim1);
}


void Check_iF_Crash(uint8_t oX, uint8_t oS, uint8_t oE)
{
	uint8_t _Ys = oS*8;
	uint8_t _Ye = oE*8;
	uint8_t showScore[10] = {0};

	if((tX <= oX+3 && tX > oX) && (tY <_Ys || (tY+9) > _Ye)){
		TIM1_IT_Stop();

		printf("%d-%d-%d-%d-%d\r\n", tX, tY, oX, oS, oE);

		crashFlag = 1;
	}

	if(tX == oX+4){
		userScore++;
		snprintf((char *)showScore, sizeof(showScore)-1, "%d", userScore);
		OLED_ShowString(60, 3, showScore, 16);
	}

}

uint8_t Check_iF_Stop(void)
{
	if(1 == crashFlag){
		crashFlag = 0;
		HAL_Delay(1500);
		if(userScore) userScore--;
		OLED_Show_BirdScore(userScore);
		userScore = 0;
		return 1;
	}
	return 0;
}

void Init_All_Obstacle(void)
{
	uint8_t i,j;
	for(i=0; i<OBSTACLE_SZ; i++){
		for(j=0; j<8; j++){
			if(0 == i){
				AllObstacle[i][j] = 0;

				if(2 == j)AllObstacle[i][2] = 3;
				if(3 == j)AllObstacle[i][3] = 5;
				if(4 == j)AllObstacle[i][4] = 1;

			}else if(1 == i){
				AllObstacle[i][j] = 0;

				if(2 == j)AllObstacle[i][2] = 2;
				if(3 == j)AllObstacle[i][3] = 4;

			}else if(2 == i){
				AllObstacle[i][j] = 0;

				if(2 == j)AllObstacle[i][2] = 5;
				if(3 == j)AllObstacle[i][3] = 7;

			}else if(3 == i){
				AllObstacle[i][j] = 0;

				if(2 == j)AllObstacle[i][2] = 1;
				if(3 == j)AllObstacle[i][3] = 3;
			}else if(4 == i){
				AllObstacle[i][j] = 0;

				if(2 == j)AllObstacle[i][2] = 3;
				if(3 == j)AllObstacle[i][3] = 5;
			}
		}
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
