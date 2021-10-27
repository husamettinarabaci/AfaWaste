/**
  ******************************************************************************
  * @file    lte.h
  * @brief   This file contains all the function prototypes for
  *          the lte.h's c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 cloud.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LTE_H__
#define __LTE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */
#define LTE_PWRKEY_ENABLE_TIME 2000
#define LTE_RESET_ENABLE_TIME 2000
#define LTE_PWRKEY_DISABLE_TIME 10000
#define LTE_GPS_FIX_WAIT_TIMEOUT 90000 // in milliseconds

typedef enum
{
	PWR_PASSIVE= 1,
	PWR_ACTIVE
} lteUartPwrStates_typedef;

typedef enum
{
	MESG_NONE= 1,
	MESG_POWERDOWN
} lteUartResMesgStates_typedef;

typedef enum
{
	HEAD_NONE= 1,
	HEAD_OK,
	HEAD_ERROR,
	HEAD_READY
} lteUartResHeadStates_typedef;

typedef struct
{
	uint8_t imeiNum[15];
	uint8_t imsiNum[15];
	uint8_t latitudeData[11];
	uint8_t longitudeData[11];
	char msgToSend[128];
	uint8_t uartResHeader;
	uint8_t uartResMessage;
	uint8_t powerState;
	uint8_t contexId;
	uint8_t sessionId;

} lteParams_typedef;

typedef struct
{
	char AT[4];
	char ATI[5];
	char AT_QPOWD[12];
	char AT_CGSN[9];
	char AT_QGPS_ON[11];
	char AT_QGPS_OFF[12];
	char AT_QGPSLOC[14];
	char AT_QIOPEN[60];
	char AT_QISEND[16];
	char AT_QICLOSE[14];
	char AT_QIACT[12];
	char AT_QIDEACT[14];
	char AT_QICSGP[29];
	char AT_CFUN[13];
	char AT_QGPSCFG[24];
	char AT_QGPSGNMEA[20];
	char AT_CIMI[9];
	char POWER_ON[8];
	char POWER_OFF[9];
} lteMessages_typedef;

extern lteMessages_typedef lteMessages;
extern lteParams_typedef lteParameters;

/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
void LTEPowerOn(void);
void LTEPowerOff(void);
void LTEInitParams(void);
void LTESendData(UART_HandleTypeDef uartObj, char msg[], uint8_t msgSize);
void LTEResponseParserBuffer(uint32_t delay, char msg[], uint8_t msgSize);
void LTEGetIMEI(void);
void LTEOpenConnection(void);
void LTEPrepToSendData(void);
void LTESendMsg(void);
void LTECloseConnection(void);
void LTEAt(void);
void LTEAti(void);
void LTEReset(void);
void LTEGPSPowerOn(void);
void LTEGPSPowerOff(void);
void LTEGPSGetData(void);
void SearchStatu();
uint8_t SearchMessage(uint8_t startInd,char msg[],uint8_t msgSize);
void LTEActiveBuffer(void);
void LTEPassiveBuffer(void);
void LTEPowerOffWithUart(void);
void LTEPdpOpen(void);
void LTEPdpClose(void);
void LTEPdpConfigure(void);
void LTEGPSNmeaConfig(void);
void LTEGetImsi(void);
void LTEGPSGetNmeaData(void);
char ConvertIdToChar(uint8_t);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __LTE_H__ */

/************************ (C) COPYRIGHT cloud *****END OF FILE****/
