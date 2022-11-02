/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00707CBB;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB13     ------> I2C2_SCL
    PB14     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PB13     ------> I2C2_SCL
    PB14     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#include "sht3x.h"

#define CRC8_POLYNOMIAL 0x31
uint8_t CheckCrc8(uint8_t* message, uint8_t initial_value)
{
    uint8_t  remainder;
    uint8_t  i = 0, j = 0;

    remainder = initial_value;

    for(j = 0; j < 2;j++)
    {
        remainder ^= message[j];

        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1)^CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

HAL_StatusTypeDef i2c_write_cmd(uint16_t cmd)
{
    uint8_t cmd_buff[2];
    cmd_buff[0] = cmd>>8;
    cmd_buff[1] = cmd;

    return HAL_I2C_Master_Transmit(&hi2c2, SHT30_ADDR_WR,cmd_buff,2,0xffff);
}

HAL_StatusTypeDef KE1_I2C_SHT31_Init()
{
	HAL_StatusTypeDef sta = HAL_OK;
    //soft reset
	sta = i2c_write_cmd(CMD_SOFT_RESET); // 复位芯片
    HAL_Delay(25);

    sta = i2c_write_cmd(CMD_MEAS_PERI_2_M);//设置工作模式

    HAL_Delay(25);

    return sta;
}

int KE1_I2C_SHT31(float *pfTemp, float *pfHumi)
{
	uint8_t read_buff[6] = {0};

	uint16_t temp_value;
	uint16_t humi_value;

	i2c_write_cmd(CMD_FETCH_DATA);//read for period mode

	//read value
	if(HAL_I2C_Master_Receive(&hi2c2, SHT30_ADDR_RD,read_buff,6,0xffff) != HAL_OK)
	{
		return HAL_ERROR;
	}

	//check crc
	if(CheckCrc8(read_buff, 0xFF) != read_buff[2] || CheckCrc8(&read_buff[3], 0xFF) != read_buff[5])
	{
		return HAL_ERROR;
	}

	//translate
	temp_value = ((uint16_t)read_buff[0]<<8)|read_buff[1];
	*pfTemp = -45 + 175*((float)temp_value/65535);

	humi_value = ((uint16_t)read_buff[3]<<8)|read_buff[4];
	*pfHumi = 100 * ((float)humi_value / 65535);

	return HAL_OK;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
