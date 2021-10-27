/**
 ******************************************************************************
 * @file    lte.c
 * @brief   This file provides code for the configuration and utils of
 *          the lte
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 cloud.
 * All rights reserved.</center></h2>
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "lte.h"
#include "usart.h"
#include "waste.h"

lteParams_typedef lteParameters;
lteMessages_typedef lteMessages;

void LTEInitParams() {
	strcpy(lteMessages.AT, "AT\r");
	strcpy(lteMessages.ATI, "ATI\r");
	strcpy(lteMessages.AT_QPOWD, "AT+QPOWD=1\r");
	strcpy(lteMessages.AT_CGSN, "AT+CGSN\r");
	strcpy(lteMessages.AT_QIOPEN, "AT+QIOPEN=1,0,\"TCP\",\"listener.aws.afatek.com.tr\",20000\r");
	lteMessages.AT_QIOPEN[10] = ConvertIdToChar(lteParameters.contexId);
	lteMessages.AT_QIOPEN[12] = ConvertIdToChar(lteParameters.sessionId);
	strcpy(lteMessages.AT_QISEND, "AT+QISEND=0,128\r");
	lteMessages.AT_QISEND[10] = ConvertIdToChar(lteParameters.sessionId);
	strcpy(lteMessages.AT_QICLOSE, "AT+QICLOSE=0\r");
	lteMessages.AT_QICLOSE[11] = ConvertIdToChar(lteParameters.sessionId);
	strcpy(lteMessages.AT_QIACT, "AT+QIACT=1\r");
	lteMessages.AT_QIACT[9] = ConvertIdToChar(lteParameters.contexId);
	strcpy(lteMessages.AT_QIDEACT, "AT+QIDEACT=1\r");
	lteMessages.AT_QIDEACT[11] = ConvertIdToChar(lteParameters.contexId);
	strcpy(lteMessages.AT_QICSGP, "AT+QICSGP=1,1,\"mgb\",\"\",\"\",3\r");
	lteMessages.AT_QICSGP[10] = ConvertIdToChar(lteParameters.contexId);
	strcpy(lteMessages.AT_QGPS_ON, "AT+QGPS=1\r");
	strcpy(lteMessages.AT_QGPS_OFF, "AT+QGPSEND\r");
	strcpy(lteMessages.AT_QGPSLOC, "AT+QGPSLOC=2\r");
	strcpy(lteMessages.AT_CFUN, "AT+CFUN=1,1\r");
	strcpy(lteMessages.AT_CIMI, "AT+CIMI\r");
	strcpy(lteMessages.AT_QGPSCFG, "AT+QGPSCFG=\"nmeasrc\",1\r");
	strcpy(lteMessages.AT_QGPSGNMEA, "AT+QGPSGNMEA=\"GGA\"\r");
	strcpy(lteMessages.POWER_ON, "POWER_ON");
	strcpy(lteMessages.POWER_OFF, "POWER_OFF");

	memset(lteParameters.latitudeData, '*', 11);
	memset(lteParameters.longitudeData, '*', 11);
}

void LTEPowerOn() {

	DebugSendData("LTE Power On", 12);
	LTEActiveBuffer();
	HAL_GPIO_WritePin(LTE_PWRKEY_GPIO_Port, LTE_PWRKEY_Pin, GPIO_PIN_SET); // LTE module power key pressed
	HAL_Delay(LTE_PWRKEY_ENABLE_TIME); // Wait about 2000 ms for process
	HAL_GPIO_WritePin(LTE_PWRKEY_GPIO_Port, LTE_PWRKEY_Pin, GPIO_PIN_RESET); // LTE module power key released
	LTEResponseParserBuffer(30000, lteMessages.POWER_ON, sizeof(lteMessages.POWER_ON));
	if (lteParameters.uartResHeader == HEAD_READY) {
		DebugSendData("LTE Power Active", 16);
		lteParameters.powerState = PWR_ACTIVE;
	}
	LTEPassiveBuffer();
}

void LTEPowerOff() {

	DebugSendData("LTE Power Off", 13);
	LTEActiveBuffer();
	HAL_GPIO_WritePin(LTE_PWRKEY_GPIO_Port, LTE_PWRKEY_Pin, GPIO_PIN_SET); // LTE module power key pressed
	HAL_Delay(LTE_PWRKEY_DISABLE_TIME); // Wait about 2000 ms for process
	HAL_GPIO_WritePin(LTE_PWRKEY_GPIO_Port, LTE_PWRKEY_Pin, GPIO_PIN_RESET); // LTE module power key released
	LTEResponseParserBuffer(15000, lteMessages.POWER_OFF, sizeof(lteMessages.POWER_OFF));
	if (lteParameters.uartResHeader == HEAD_OK && lteParameters.uartResMessage == MESG_POWERDOWN) {
		DebugSendData("LTE Power Passive", 17);
		lteParameters.powerState = PWR_PASSIVE;
	}
	LTEPassiveBuffer();
}

void LTEReset() {
	DebugSendData("LTE Reset", 9);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_CFUN, sizeof(lteMessages.AT_CFUN));
	LTEResponseParserBuffer(20000, lteMessages.AT_CFUN, sizeof(lteMessages.AT_CFUN));
	LTEPassiveBuffer();
}

void LTEPowerOffWithUart() {
	DebugSendData("LTE Power Off With Uart", 23);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QPOWD, sizeof(lteMessages.AT_QPOWD));
	LTEResponseParserBuffer(15000, lteMessages.AT_QPOWD, sizeof(lteMessages.AT_QPOWD));
	if (lteParameters.uartResHeader == HEAD_OK && lteParameters.uartResMessage == MESG_POWERDOWN) {
		DebugSendData("LTE Power Passive", 17);
		lteParameters.powerState = PWR_PASSIVE;
	}

	LTEPassiveBuffer();
}

void LTEGetIMEI() {
	DebugSendData("LTE Get IMEI", 12);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_CGSN, sizeof(lteMessages.AT_CGSN));
	LTEResponseParserBuffer(2000, lteMessages.AT_CGSN, sizeof(lteMessages.AT_CGSN));
	LTEPassiveBuffer();
}

void LTEGetImsi() {
	DebugSendData("LTE Get IMSI", 12);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_CIMI, sizeof(lteMessages.AT_CIMI));
	LTEResponseParserBuffer(2000, lteMessages.AT_CIMI, sizeof(lteMessages.AT_CIMI));
	LTEPassiveBuffer();
}

void LTEOpenConnection() {
	DebugSendData("LTE Open Connection", 19);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QIOPEN, sizeof(lteMessages.AT_QIOPEN));
	LTEResponseParserBuffer(1000, lteMessages.AT_QIOPEN, sizeof(lteMessages.AT_QIOPEN));
	LTEPassiveBuffer();
}

void LTEPrepToSendData() {
	DebugSendData("LTE Prep To Send Data", 21);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QISEND, sizeof(lteMessages.AT_QISEND));
	LTEResponseParserBuffer(2000, lteMessages.AT_QISEND, sizeof(lteMessages.AT_QISEND));
	LTEPassiveBuffer();
}

void LTEAt() {
	DebugSendData("LTE At", 6);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT, sizeof(lteMessages.AT));
	LTEResponseParserBuffer(5000, lteMessages.AT, sizeof(lteMessages.AT));
	LTEPassiveBuffer();
}

void LTEAti() {
	DebugSendData("LTE Ati", 7);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.ATI, sizeof(lteMessages.ATI));
	LTEResponseParserBuffer(5000, lteMessages.ATI, sizeof(lteMessages.ATI));
	LTEPassiveBuffer();
}

void LTECloseConnection() {
	DebugSendData("LTE Close Connection", 20);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QICLOSE, sizeof(lteMessages.AT_QICLOSE));
	LTEResponseParserBuffer(15000, lteMessages.AT_QICLOSE, sizeof(lteMessages.AT_QICLOSE));
	LTEPassiveBuffer();
}

void LTEPdpOpen() {
	DebugSendData("LTE Pdp Open", 12);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QIACT, sizeof(lteMessages.AT_QIACT));
	LTEResponseParserBuffer(10000, lteMessages.AT_QIACT, sizeof(lteMessages.AT_QIACT));
	LTEPassiveBuffer();
}

void LTEPdpClose() {
	DebugSendData("LTE Pdp Close", 13);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QIDEACT, sizeof(lteMessages.AT_QIDEACT));
	LTEResponseParserBuffer(10000, lteMessages.AT_QIDEACT, sizeof(lteMessages.AT_QIDEACT));
	LTEPassiveBuffer();
}

void LTEPdpConfigure() {
	DebugSendData("LTE Pdp Configure", 17);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QICSGP, sizeof(lteMessages.AT_QICSGP));
	LTEResponseParserBuffer(2000, lteMessages.AT_QICSGP, sizeof(lteMessages.AT_QICSGP));
	LTEPassiveBuffer();
}

void LTESendMsg() {
	DebugSendData("LTE Send Msg", 12);
	LTESendData(huart1, lteParameters.msgToSend, 128);
}

void LTEGPSPowerOn() {
	DebugSendData("LTE Gps Power On", 16);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QGPS_ON, sizeof(lteMessages.AT_QGPS_ON));
	LTEResponseParserBuffer(1000, lteMessages.AT_QGPS_ON, sizeof(lteMessages.AT_QGPS_ON));
	LTEPassiveBuffer();
	HAL_Delay(LTE_GPS_FIX_WAIT_TIMEOUT);
}

void LTEGPSPowerOff() {
	DebugSendData("LTE Gps Power Off", 17);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QGPS_OFF, sizeof(lteMessages.AT_QGPS_OFF));
	LTEResponseParserBuffer(1000, lteMessages.AT_QGPS_OFF, sizeof(lteMessages.AT_QGPS_OFF));
	LTEPassiveBuffer();
}

void LTEGPSNmeaConfig() {
	DebugSendData("LTE Gps Nmea Config", 19);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QGPSCFG, sizeof(lteMessages.AT_QGPSCFG));
	LTEResponseParserBuffer(2000, lteMessages.AT_QGPSCFG, sizeof(lteMessages.AT_QGPSCFG));
	LTEPassiveBuffer();
}

void LTEGPSGetNmeaData() {
	DebugSendData("LTE Gps Get Nmea Data", 21);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QGPSGNMEA, sizeof(lteMessages.AT_QGPSGNMEA));
	LTEResponseParserBuffer(2000, lteMessages.AT_QGPSGNMEA, sizeof(lteMessages.AT_QGPSGNMEA));
	LTEPassiveBuffer();
}

void LTEGPSGetData() {
	DebugSendData("LTE Gps Get Data", 16);
	LTEActiveBuffer();
	LTESendData(huart1, lteMessages.AT_QGPSLOC, sizeof(lteMessages.AT_QGPSLOC));
	LTEResponseParserBuffer(2000, lteMessages.AT_QGPSLOC, sizeof(lteMessages.AT_QGPSLOC));
	LTEPassiveBuffer();
}

void LTEActiveBuffer() {
	uartLTEParameters.active = 0;
	memset(uartLTEParameters.rxBuffer, '*', 256);
	uartLTEParameters.rxDataCounter = 0;
	uartLTEParameters.active = 1;
}
void LTEPassiveBuffer() {
	uartLTEParameters.active = 0;
	memset(uartLTEParameters.rxBuffer, '*', 256);
	uartLTEParameters.rxDataCounter = 0;
	uartLTEParameters.active = 0;
}
void LTESendData(UART_HandleTypeDef uartObj, char msg[], uint8_t msgSize) {
	HAL_UART_Transmit(&uartObj, (uint8_t*) msg, msgSize, HAL_MAX_DELAY); // Send message to the module
}
void LTEResponseParserBuffer(uint32_t delay, char msg[], uint8_t msgSize) {
	lteParameters.uartResHeader = HEAD_NONE;
	lteParameters.uartResMessage = MESG_NONE;
	HAL_Delay(delay);
	DebugSendData(uartLTEParameters.rxBuffer, 122);
	SearchStatu();
	if (lteParameters.uartResHeader == HEAD_NONE) {
		DebugSendData("Header : None", 13);
	} else if (lteParameters.uartResHeader == HEAD_OK) {
		DebugSendData("Header : OK", 12);
	} else if (lteParameters.uartResHeader == HEAD_ERROR) {
		DebugSendData("Header : Error", 14);
	} else if (lteParameters.uartResHeader == HEAD_READY) {
		DebugSendData("Header : Ready", 14);
	} else {
		DebugSendData("Header : Not Found", 18);
	}
	int comOk = 0;
	for (int i = 0; i < msgSize; ++i) {
		if (uartLTEParameters.rxBuffer[i] == msg[i]) {
			comOk = 1;
			break;
		}
	}

	if (msg == lteMessages.AT_CGSN) {
		if (comOk && lteParameters.uartResHeader == HEAD_OK) {
			for (int i = 0; i < 15; ++i) {
				lteParameters.imeiNum[i] = uartLTEParameters.rxBuffer[sizeof(lteMessages.AT_CGSN) + 1 + i];
			}
		} else {
		}
	}

	if (msg == lteMessages.AT_CIMI) {
		if (comOk && lteParameters.uartResHeader == HEAD_OK) {
			for (int i = 0; i < 15; ++i) {
				lteParameters.imsiNum[i] = uartLTEParameters.rxBuffer[sizeof(lteMessages.AT_CIMI) + 1 + i];
			}
		} else {
		}
	}

	if (msg == lteMessages.AT_QPOWD || msg == lteMessages.POWER_ON || msg == lteMessages.POWER_OFF) {
		if (SearchMessage(msgSize, "DOWN", 4)) {
			lteParameters.uartResMessage = MESG_POWERDOWN;
			DebugSendData("Message : Power Down", 20);
		}
	}
}

uint8_t SearchMessage(uint8_t startInd, char msg[], uint8_t msgSize) {
	uint8_t msgInd = 0;
	for (int i = startInd; i < sizeof(uartLTEParameters.rxBuffer); ++i) {
		if (uartLTEParameters.rxBuffer[i] == msg[msgInd]) {
			msgInd++;
		} else {
			msgInd = 0;
		}

		if (msgInd == msgSize) {
			return 1;
		}
	}

	return 0;
}

void SearchStatu() {
	lteParameters.uartResHeader = HEAD_NONE;

	char msgOk[] = "OK";
	char msgErr[] = "ERROR";
	char msgRdy[] = "RDY";

	if (SearchMessage(0, msgOk, 2))
		lteParameters.uartResHeader = HEAD_OK;
	if (SearchMessage(0, msgErr, 5))
		lteParameters.uartResHeader = HEAD_ERROR;
	if (SearchMessage(0, msgRdy, 3))
		lteParameters.uartResHeader = HEAD_READY;
}

char ConvertIdToChar(uint8_t id) {
	char retVal = '1';
	switch (id) {
	case 0:
		retVal = '0';
		break;
	case 1:
		retVal = '1';
		break;
	case 2:
		retVal = '2';
		break;
	case 3:
		retVal = '3';
		break;
	case 4:
		retVal = '4';
		break;
	case 5:
		retVal = '5';
		break;
	case 6:
		retVal = '6';
		break;
	case 7:
		retVal = '7';
		break;
	case 8:
		retVal = '8';
		break;
	case 9:
		retVal = '9';
		break;
	default:
		break;
	}
	return retVal;
}

/************************ (C) COPYRIGHT cloud *****END OF FILE****/
