/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SetSendData(void);
void MainProc(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define UART_RX_LTE_Pin GPIO_PIN_10
#define UART_RX_LTE_GPIO_Port GPIOA
#define UART_TX_LTE_Pin GPIO_PIN_9
#define UART_TX_LTE_GPIO_Port GPIOA
#define LTE_RESET_Pin GPIO_PIN_1
#define LTE_RESET_GPIO_Port GPIOA
#define LTE_PWRKEY_Pin GPIO_PIN_0
#define LTE_PWRKEY_GPIO_Port GPIOA
#define ULT_ENABLE_Pin GPIO_PIN_11
#define ULT_ENABLE_GPIO_Port GPIOB
#define AT_MODE_Pin GPIO_PIN_4
#define AT_MODE_GPIO_Port GPIOA
#define UART_TX_DBG_Pin GPIO_PIN_2
#define UART_TX_DBG_GPIO_Port GPIOA
#define ULT_EMIT_Pin GPIO_PIN_10
#define ULT_EMIT_GPIO_Port GPIOB
#define ULT_ECHO_Pin GPIO_PIN_5
#define ULT_ECHO_GPIO_Port GPIOA
#define ULT_ECHO_EXTI_IRQn EXTI4_15_IRQn
#define UART_RX_DBG_Pin GPIO_PIN_3
#define UART_RX_DBG_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
