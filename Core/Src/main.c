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
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usr.h"
#include "waste.h"
#include "lte.h"
#include <stdio.h>

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

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

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
	MX_RTC_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_ADC_Init();
	/* USER CODE BEGIN 2 */
	UartSettings();
	WasteInit();

	HAL_Delay(5000);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1) {
		DebugSendData("While Start", 11);
		HAL_Delay(1000);
		if (rtcParameters.wakeUpFlag) {
			MainProc();
		}
		LTEPowerOffWithUart();
		HAL_Delay(1000);
		if (lteParameters.uartResHeader == HEAD_READY) {
			LTEPowerOff();
		}
		rtcParameters.wakeUpFlag = 0;
		DebugSendData("Sleep Mode", 10);
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
		HAL_ResumeTick();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_RTC;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void MainProc() {
	DebugSendData("Wake Up", 7);
	adcParameters.adcReadEnableFlag = 1;
	DebugSendData("Enable Adc", 10);
	HAL_ADC_Start_IT(&hadc);
	HAL_Delay(3000);
	wasteParameters.voltVal = adcParameters.supplyVoltage;
	wasteParameters.tempVal = adcParameters.tempCelcius;
	if (wasteParameters.tempVal > 100) {
		wasteParameters.tempVal = wasteParameters.tempVal % 100;
	}
	DebugSendData("Disable Adc", 11);
	adcParameters.adcReadEnableFlag = 0;
	if (wasteParameters.setupMode) {

		wasteParameters.setupCount++;

		if (wasteParameters.setupCount >= SETUP_WORK_PERIOD) {
			wasteParameters.setupMode = 0;
		}
	}
	if (wasteParameters.setupMode) {
		DebugSendData("Setup Mode Enable", 17);
	} else {
		DebugSendData("Setup Mode Disable", 18);
	}
	if (wasteParameters.wakeUpCount == 0 || wasteParameters.tempVal >= CRITICAL_TEMPERATURE || (wasteParameters.setupMode && wasteParameters.setupCount % 2 == 1)) {

		wasteParameters.wakeUpCount = 0;
		LTEInitParams();
		UsrEnable();
		HAL_Delay(1000);
		UsrMeasure();
		HAL_Delay(1000);
		UsrDisable();
		HAL_Delay(1000);
		LTEPowerOn();
		HAL_Delay(1000);
		if (lteParameters.uartResMessage == MESG_POWERDOWN) {
			DebugSendData("Power On Again", 14);
			LTEPowerOn();
			HAL_Delay(1000);
		}
		LTEReset();
		HAL_Delay(1000);
		for (int i = 0; i < 10; ++i) {
			LTEAt();
			HAL_Delay(1000);
			if (lteParameters.uartResHeader == HEAD_OK) {
				break;
			}
		}
//		LTEAti();
//		HAL_Delay(1000);
		LTEGetIMEI();
		HAL_Delay(1000);
		LTEGetImsi();
		HAL_Delay(1000);
//		LTEGPSPowerOn();
//		HAL_Delay(1000);
//		LTEGPSNmeaConfig();
//		HAL_Delay(1000);
//		for (int i = 0; i < 1000; ++i) {
//			LTEGPSGetNmeaData();
//			HAL_Delay(1000);
//		}
//		LTEGPSPowerOff();
//		HAL_Delay(1000);
		SetSendData();
		HAL_Delay(1000);
		LTEPdpConfigure();
		HAL_Delay(500);
		LTEPdpOpen();
		HAL_Delay(1000);
		LTEOpenConnection();
		HAL_Delay(1000);
		LTEPrepToSendData();
		HAL_Delay(1000);
		LTESendMsg();
		HAL_Delay(1000);
		LTECloseConnection();
		HAL_Delay(1000);
		LTEPdpClose();
		HAL_Delay(1000);
		LTEPowerOff();
		HAL_Delay(1000);

		lteParameters.contexId++;
		lteParameters.sessionId++;
		if (lteParameters.contexId >= 9)
			lteParameters.contexId = 1;
		if (lteParameters.contexId >= 9)
			lteParameters.sessionId = 0;

	}

	wasteParameters.wakeUpCount++;
	if (wasteParameters.wakeUpCount >= NORMAL_WORK_PERIOD) {
		wasteParameters.wakeUpCount = 0;
	}
}
void SetSendData() {
	memset(lteParameters.msgToSend, '*', 128);
	DebugSendData("Set Send Data", 13);
	lteParameters.msgToSend[0] = 'U';
	lteParameters.msgToSend[1] = 'L';
	lteParameters.msgToSend[2] = 'T';
	lteParameters.msgToSend[3] = '#';

	for (int i = 4; i < 19; i++)
		sprintf(&lteParameters.msgToSend[i], "%c", lteParameters.imeiNum[i - 4]);

	lteParameters.msgToSend[19] = '#';
	sprintf(&lteParameters.msgToSend[20], "%d", ((uint8_t) (wasteParameters.tempVal % 1000) - (uint8_t) (wasteParameters.tempVal % 100)) / 100);
	sprintf(&lteParameters.msgToSend[21], "%d", ((uint8_t) (wasteParameters.tempVal % 100) - (uint8_t) (wasteParameters.tempVal % 10)) / 10);
	sprintf(&lteParameters.msgToSend[22], "%d", (uint8_t) (wasteParameters.tempVal % 10));

	lteParameters.msgToSend[23] = '#';
	sprintf(&lteParameters.msgToSend[24], "%d", ((uint16_t) (wasteParameters.voltVal) - (uint16_t) (wasteParameters.voltVal % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[25], "%d", ((uint16_t) (wasteParameters.voltVal % 1000) - (uint16_t) (wasteParameters.voltVal % 100)) / 100);
	sprintf(&lteParameters.msgToSend[26], "%d", ((uint16_t) (wasteParameters.voltVal % 100) - (uint16_t) (wasteParameters.voltVal % 10)) / 10);
	sprintf(&lteParameters.msgToSend[27], "%d", (uint16_t) (wasteParameters.voltVal % 10));

	lteParameters.msgToSend[28] = '#';

	for (int i = 0; i < 11; i++)
		sprintf(&lteParameters.msgToSend[29 + i], "%c", lteParameters.latitudeData[i]);

	lteParameters.msgToSend[40] = '#';

	for (int i = 0; i < 11; i++)
		sprintf(&lteParameters.msgToSend[41 + i], "%c", lteParameters.longitudeData[i]);

	lteParameters.msgToSend[52] = '#';
	sprintf(&lteParameters.msgToSend[53], "%d", ((uint16_t) (usrParameters.usrMeasArray[0] % 100000) - (uint16_t) (usrParameters.usrMeasArray[0] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[54], "%d", ((uint16_t) (usrParameters.usrMeasArray[0] % 10000) - (uint16_t) (usrParameters.usrMeasArray[0] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[55], "%d", ((uint16_t) (usrParameters.usrMeasArray[0] % 1000) - (uint16_t) (usrParameters.usrMeasArray[0] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[56], "%d", ((uint16_t) (usrParameters.usrMeasArray[0] % 100) - (uint16_t) (usrParameters.usrMeasArray[0] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[57], "%d", (uint16_t) (usrParameters.usrMeasArray[0] % 10));
	lteParameters.msgToSend[58] = '#';
	sprintf(&lteParameters.msgToSend[59], "%d", ((uint16_t) (usrParameters.usrMeasArray[1] % 100000) - (uint16_t) (usrParameters.usrMeasArray[1] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[60], "%d", ((uint16_t) (usrParameters.usrMeasArray[1] % 10000) - (uint16_t) (usrParameters.usrMeasArray[1] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[61], "%d", ((uint16_t) (usrParameters.usrMeasArray[1] % 1000) - (uint16_t) (usrParameters.usrMeasArray[1] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[62], "%d", ((uint16_t) (usrParameters.usrMeasArray[1] % 100) - (uint16_t) (usrParameters.usrMeasArray[1] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[63], "%d", (uint16_t) (usrParameters.usrMeasArray[1] % 10));
	lteParameters.msgToSend[64] = '#';
	sprintf(&lteParameters.msgToSend[65], "%d", ((uint16_t) (usrParameters.usrMeasArray[2] % 100000) - (uint16_t) (usrParameters.usrMeasArray[2] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[66], "%d", ((uint16_t) (usrParameters.usrMeasArray[2] % 10000) - (uint16_t) (usrParameters.usrMeasArray[2] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[67], "%d", ((uint16_t) (usrParameters.usrMeasArray[2] % 1000) - (uint16_t) (usrParameters.usrMeasArray[2] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[68], "%d", ((uint16_t) (usrParameters.usrMeasArray[2] % 100) - (uint16_t) (usrParameters.usrMeasArray[2] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[69], "%d", (uint16_t) (usrParameters.usrMeasArray[2] % 10));
	lteParameters.msgToSend[70] = '#';
	sprintf(&lteParameters.msgToSend[71], "%d", ((uint16_t) (usrParameters.usrMeasArray[3] % 100000) - (uint16_t) (usrParameters.usrMeasArray[3] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[72], "%d", ((uint16_t) (usrParameters.usrMeasArray[3] % 10000) - (uint16_t) (usrParameters.usrMeasArray[3] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[73], "%d", ((uint16_t) (usrParameters.usrMeasArray[3] % 1000) - (uint16_t) (usrParameters.usrMeasArray[3] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[74], "%d", ((uint16_t) (usrParameters.usrMeasArray[3] % 100) - (uint16_t) (usrParameters.usrMeasArray[3] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[75], "%d", (uint16_t) (usrParameters.usrMeasArray[3] % 10));
	lteParameters.msgToSend[76] = '#';
	sprintf(&lteParameters.msgToSend[77], "%d", ((uint16_t) (usrParameters.usrMeasArray[4] % 100000) - (uint16_t) (usrParameters.usrMeasArray[4] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[78], "%d", ((uint16_t) (usrParameters.usrMeasArray[4] % 10000) - (uint16_t) (usrParameters.usrMeasArray[4] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[79], "%d", ((uint16_t) (usrParameters.usrMeasArray[4] % 1000) - (uint16_t) (usrParameters.usrMeasArray[4] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[80], "%d", ((uint16_t) (usrParameters.usrMeasArray[4] % 100) - (uint16_t) (usrParameters.usrMeasArray[4] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[81], "%d", (uint16_t) (usrParameters.usrMeasArray[4] % 10));
	lteParameters.msgToSend[82] = '#';
	sprintf(&lteParameters.msgToSend[83], "%d", ((uint16_t) (usrParameters.usrMeasArray[5] % 100000) - (uint16_t) (usrParameters.usrMeasArray[5] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[84], "%d", ((uint16_t) (usrParameters.usrMeasArray[5] % 10000) - (uint16_t) (usrParameters.usrMeasArray[5] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[85], "%d", ((uint16_t) (usrParameters.usrMeasArray[5] % 1000) - (uint16_t) (usrParameters.usrMeasArray[5] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[86], "%d", ((uint16_t) (usrParameters.usrMeasArray[5] % 100) - (uint16_t) (usrParameters.usrMeasArray[5] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[87], "%d", (uint16_t) (usrParameters.usrMeasArray[5] % 10));
	lteParameters.msgToSend[88] = '#';
	sprintf(&lteParameters.msgToSend[89], "%d", ((uint16_t) (usrParameters.usrMeasArray[6] % 100000) - (uint16_t) (usrParameters.usrMeasArray[6] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[90], "%d", ((uint16_t) (usrParameters.usrMeasArray[6] % 10000) - (uint16_t) (usrParameters.usrMeasArray[6] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[91], "%d", ((uint16_t) (usrParameters.usrMeasArray[6] % 1000) - (uint16_t) (usrParameters.usrMeasArray[6] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[92], "%d", ((uint16_t) (usrParameters.usrMeasArray[6] % 100) - (uint16_t) (usrParameters.usrMeasArray[6] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[93], "%d", (uint16_t) (usrParameters.usrMeasArray[6] % 10));
	lteParameters.msgToSend[94] = '#';
	sprintf(&lteParameters.msgToSend[95], "%d", ((uint16_t) (usrParameters.usrMeasArray[7] % 100000) - (uint16_t) (usrParameters.usrMeasArray[7] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[96], "%d", ((uint16_t) (usrParameters.usrMeasArray[7] % 10000) - (uint16_t) (usrParameters.usrMeasArray[7] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[97], "%d", ((uint16_t) (usrParameters.usrMeasArray[7] % 1000) - (uint16_t) (usrParameters.usrMeasArray[7] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[98], "%d", ((uint16_t) (usrParameters.usrMeasArray[7] % 100) - (uint16_t) (usrParameters.usrMeasArray[7] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[99], "%d", (uint16_t) (usrParameters.usrMeasArray[7] % 10));
	lteParameters.msgToSend[100] = '#';
	sprintf(&lteParameters.msgToSend[101], "%d", ((uint16_t) (usrParameters.usrMeasArray[8] % 100000) - (uint16_t) (usrParameters.usrMeasArray[8] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[102], "%d", ((uint16_t) (usrParameters.usrMeasArray[8] % 10000) - (uint16_t) (usrParameters.usrMeasArray[8] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[103], "%d", ((uint16_t) (usrParameters.usrMeasArray[8] % 1000) - (uint16_t) (usrParameters.usrMeasArray[8] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[104], "%d", ((uint16_t) (usrParameters.usrMeasArray[8] % 100) - (uint16_t) (usrParameters.usrMeasArray[8] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[105], "%d", (uint16_t) (usrParameters.usrMeasArray[8] % 10));
	lteParameters.msgToSend[106] = '#';
	sprintf(&lteParameters.msgToSend[107], "%d", ((uint16_t) (usrParameters.usrMeasArray[9] % 100000) - (uint16_t) (usrParameters.usrMeasArray[9] % 10000)) / 10000);
	sprintf(&lteParameters.msgToSend[108], "%d", ((uint16_t) (usrParameters.usrMeasArray[9] % 10000) - (uint16_t) (usrParameters.usrMeasArray[9] % 1000)) / 1000);
	sprintf(&lteParameters.msgToSend[109], "%d", ((uint16_t) (usrParameters.usrMeasArray[9] % 1000) - (uint16_t) (usrParameters.usrMeasArray[9] % 100)) / 100);
	sprintf(&lteParameters.msgToSend[110], "%d", ((uint16_t) (usrParameters.usrMeasArray[9] % 100) - (uint16_t) (usrParameters.usrMeasArray[9] % 10)) / 10);
	sprintf(&lteParameters.msgToSend[111], "%d", (uint16_t) (usrParameters.usrMeasArray[9] % 10));

	lteParameters.msgToSend[112] = '#';
	for (int i = 113; i < 128; i++)
		sprintf(&lteParameters.msgToSend[i], "%c", lteParameters.imsiNum[i - 113]);

	DebugSendData(lteParameters.msgToSend, 128);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
