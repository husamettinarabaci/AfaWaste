/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "rtc.h"
#include "lte.h"

adcParams_typedef adcParameters;

/* USER CODE END 0 */

ADC_HandleTypeDef hadc;

/* ADC init function */
void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_COMP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
//	if(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC))
//	{
//		ADC_Raw[0] = HAL_ADC_GetValue(hadc);
////		indx++;
//
//	}
//
////	if(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOS))
////	{
////		indx = 0;
//
////		supVoltage = 3300 * (*VREFINT_CAL_ADDR) / ADC_Raw[1];
//
//		calcTemp = 0;
//		calcTemp = (((int32_t)ADC_Raw[0]) - (int32_t) *TEMP30_CAL_ADDR);
//		calcTemp = calcTemp * (int32_t)(130 - 30);
//		calcTemp = calcTemp / (int32_t)(*TEMP130_CAL_ADDR - *TEMP30_CAL_ADDR);
//		tempinC = calcTemp + 30;
//		HAL_ADC_Start_IT(hadc);
////	}

	if(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC))
	{
		adcParameters.adcRawData[adcParameters.adcIndx] = HAL_ADC_GetValue(hadc);
		adcParameters.adcIndx++;
	}

	if(adcParameters.adcIndx >= 2)
	{
		adcParameters.adcIndx = 0;

		volatile int32_t tmpVal = 0;
		volatile int32_t volVal = 0;

		if((int32_t)adcParameters.adcRawData[0] > (int32_t)adcParameters.adcRawData[1])
		{
			tmpVal = (int32_t)adcParameters.adcRawData[1];
			volVal = (int32_t)adcParameters.adcRawData[0];
		}
		else
		{
			tmpVal = (int32_t)adcParameters.adcRawData[0];
			volVal = (int32_t)adcParameters.adcRawData[1];
		}

		adcParameters.supplyVoltage = 3300 * (*VREFINT_CAL_ADDR) / volVal;

		volatile int32_t calcTemp = 0;
		calcTemp = (((int32_t)tmpVal) - (int32_t) *TEMP30_CAL_ADDR);
		calcTemp = calcTemp * (int32_t)(130 - 30);
		calcTemp = calcTemp / (int32_t)(*TEMP130_CAL_ADDR - *TEMP30_CAL_ADDR);
		adcParameters.tempCelcius = calcTemp + 30;

		if(adcParameters.adcReadEnableFlag)
		{
			HAL_ADC_Start_IT(hadc);
		}
	}

}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
