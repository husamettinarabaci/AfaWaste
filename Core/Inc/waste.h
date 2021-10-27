/**
  ******************************************************************************
  * @file    waste.h
  * @brief   This file contains all the function prototypes for
  *          the waste.h's c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 cloud.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WASTE_H__
#define __WASTE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */
#define NORMAL_WORK_PERIOD 48
#define SETUP_WORK_PERIOD 6
#define CRITICAL_TEMPERATURE 80

typedef struct
{
	uint8_t wakeUpCount;
	uint16_t voltVal;
	uint8_t tempVal;
	uint8_t setupCount;
	uint8_t setupMode;
} waste_typedef;

typedef struct
{
	char MEESAGE[138];
} debugMessages_typedef;

extern debugMessages_typedef debugMessages;

/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
void WasteInit(void);
void DebugSendData(const char * msg,uint8_t msgSize );
/* USER CODE END Prototypes */

/* USER CODE BEGIN EV */
extern waste_typedef wasteParameters;

/* USER CODE END EV */

#ifdef __cplusplus
}
#endif

#endif /* __WASTE_H__ */

/************************ (C) COPYRIGHT cloud *****END OF FILE****/
