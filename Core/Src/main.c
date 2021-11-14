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
	DebugSendData("Start App", 9);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		DebugSendData("While Start", 11);
		HAL_Delay(1000);
		if (rtcParameters.wakeUpFlag) {
			MainProc();
		}
		rtcParameters.wakeUpFlag = 0;
		DebugSendData("Sleep Mode", 10);
		HAL_SuspendTick();
		HAL_PWR_EnterSTOPMode( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
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
	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
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
	wasteParameters.gpsAlarmCount++;
	if (wasteParameters.gpsAlarmCount >= ALARM_WORK_PERIOD) {
		wasteParameters.gpsAlarmCount = 0;
	}
	if (wasteParameters.setupMode) {
		DebugSendData("Setup Mode Enable", 17);
	} else {
		DebugSendData("Setup Mode Disable", 18);
	}
	if (wasteParameters.preDataSendError) {
		DebugSendData("Prev Data Send Error", 20);
	} else {
		DebugSendData("Prev Data Send Success", 22);
	}
	if (wasteParameters.gpsAlarm) {
		DebugSendData("Gps Alarm Enable", 16);
	} else {
		DebugSendData("Gps Alarm Disable", 17);
	}
	if (wasteParameters.ultSensCount == 0) {
		if (wasteParameters.ultSensIndex >= USR_MEASURE_TOTAL_COUNT) {
			wasteParameters.ultSensIndex = 0;
		}
		if (wasteParameters.ultSensIndex == 0) {
			for (int i = 0; i < usrParameters.measureRepTotalCount; ++i) {
				usrParameters.usrMeasTotalArray[i] = 0;
			}
		}

		UsrEnable();
		HAL_Delay(1000);
		UsrMeasure();
		HAL_Delay(1000);
		UsrDisable();
		HAL_Delay(1000);
		wasteParameters.ultSensIndex++;
	}
	wasteParameters.ultSensCount++;
	if (wasteParameters.ultSensCount >= ULTL_WORK_PERIOD) {
		wasteParameters.ultSensCount = 0;
	}
	if (wasteParameters.wakeUpCount == 0 || wasteParameters.tempVal >= CRITICAL_TEMPERATURE || (wasteParameters.setupMode) || (wasteParameters.preDataSendError == 1 && wasteParameters.wakeUpCount == ALARM_WORK_PERIOD) || wasteParameters.gpsAlarm == 1) {

		wasteParameters.wakeUpCount = 0;
		LTEInitParams();
		DebugSendData("LTE Op Start", 12);
		if (wasteParameters.gpsAlarm == 0) {
			LTEEnable();
			HAL_Delay(1000);
			LTEPowerOn();
			HAL_Delay(1000);
			if (lteParameters.uartResMessage == MESG_POWERDOWN) {
				DebugSendData("Power On Again", 14);
				LTEPowerOn();
				HAL_Delay(1000);
			}
		}
		wasteParameters.preDataSendError = 1;
		LTEAt();
		HAL_Delay(1000);
		LTEGetIMEI();
		HAL_Delay(1000);
		LTEGetImsi();
		HAL_Delay(1000);
		SetSendData();
		HAL_Delay(1000);
		LTEPdpConfigure();
		HAL_Delay(1000);
		LTEPdpOpen();
		HAL_Delay(1000);
		LTEOpenConnection();
		HAL_Delay(1000);
		LTEPrepToSendData();
		HAL_Delay(1000);
		LTESendMsg();
		HAL_Delay(1000);
		LTEReceiveMsg();
		HAL_Delay(1000);
		LTECloseConnection();
		HAL_Delay(1000);
		LTEPdpClose();
		HAL_Delay(1000);
		if (wasteParameters.gpsAlarm == 0) {
			LTEPowerOff();
			HAL_Delay(1000);
			LTEPowerOffWithUart();
			HAL_Delay(1000);
			LTEDisable();
		}
		DebugSendData("LTE Op Stop", 11);
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
	memset(lteParameters.msgToSend, '*', 215);
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
	for (int i = 53; i < 68; i++)
		sprintf(&lteParameters.msgToSend[i], "%c", lteParameters.imsiNum[i - 53]);

	lteParameters.msgToSend[68] = '#';
	sprintf(&lteParameters.msgToSend[69], "%d", ((uint16_t) (wasteParameters.ultSensIndex % 100) - (uint16_t) (wasteParameters.ultSensIndex % 10)) / 10);
	sprintf(&lteParameters.msgToSend[70], "%d", (uint16_t) (wasteParameters.ultSensIndex % 10));
	uint8_t ind = 71;
	for (uint16_t cnt = 0; cnt < usrParameters.measureRepTotalCount; cnt++) {
		lteParameters.msgToSend[ind] = '#';
		ind++;
		sprintf(&lteParameters.msgToSend[ind], "%d", ((uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 100000) - (uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 10000)) / 10000);
		ind++;
		sprintf(&lteParameters.msgToSend[ind], "%d", ((uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 10000) - (uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 1000)) / 1000);
		ind++;
		sprintf(&lteParameters.msgToSend[ind], "%d", ((uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 1000) - (uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 100)) / 100);
		ind++;
		sprintf(&lteParameters.msgToSend[ind], "%d", ((uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 100) - (uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 10)) / 10);
		ind++;
		sprintf(&lteParameters.msgToSend[ind], "%d", (uint16_t) (usrParameters.usrMeasTotalArray[cnt] % 10));
		ind++;
	}

	DebugSendData(lteParameters.msgToSend, 215);
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
