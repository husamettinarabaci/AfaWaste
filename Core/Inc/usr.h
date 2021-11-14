/**
  ******************************************************************************
  * @file    usr.h
  * @brief   This file contains all the function prototypes for
  *          the usr.h's c file (Ultrasonic Sonar Ranging IC)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 cloud.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USR_H__
#define __USR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */
#define USR_NEAR_MEAS_TIMEOUT 2000
#define USR_FAR_MEAS_TIMEOUT 28000
#define USR_PWRON_STAB_TIMEOUT 100
#define USR_PWROFF_TIMEOUT 100
#define USR_EMIT_PROC_TIMEOUT 500
#define USR_MEASURE_COUNT 10
#define USR_MEASURE_TOTAL_COUNT 24


typedef struct
{
	uint8_t usrReadyRead;
	uint16_t measureRepCount;
	uint16_t measureRepTotalCount;
	uint16_t measurementValue;
	uint16_t usrMeasArray[USR_MEASURE_COUNT];
	uint16_t usrMeasTotalArray[USR_MEASURE_TOTAL_COUNT];
	uint8_t usrMeasured;
} usrParams_typedef;

/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
void UsrEnable(void);
void UsrDisable(void);
void UsrEmit(void);
void UsrMeasure(void);

/* USER CODE END Prototypes */

/* USER CODE BEGIN EV */
extern usrParams_typedef usrParameters;

/* USER CODE END EV */

#ifdef __cplusplus
}
#endif

#endif /* __USR_H__ */

/************************ (C) COPYRIGHT cloud *****END OF FILE****/
