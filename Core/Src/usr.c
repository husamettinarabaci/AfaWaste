/**
  ******************************************************************************
  * @file    usr.c
  * @brief   This file provides code for the configuration and utils of
  *          the "usr" (Ultrasonic Sonar Ranging IC)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 cloud.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usr.h"
#include "usart.h"
#include "tim.h"
#include "waste.h"

usrParams_typedef usrParameters;

/* ------- ULT SENSOR ENABLE -------------------- */
void UsrEnable()
{
	HAL_GPIO_WritePin(ULT_ENABLE_GPIO_Port, ULT_ENABLE_Pin, GPIO_PIN_SET); // Ultrasonic Sensor Power On
	HAL_Delay(USR_PWRON_STAB_TIMEOUT); // Wait 100 ms for power stabilization
DebugSendData("Usr Enable",10);

}
/* --------------- End of the ULT SENSOR ENABLE ---------------- */

/* ------- ULT SENSOR DISABLE -------------------- */
void UsrDisable()
{
	HAL_GPIO_WritePin(ULT_ENABLE_GPIO_Port, ULT_ENABLE_Pin, GPIO_PIN_RESET); // Ultrasonic Sensor Power Off
	HAL_Delay(USR_PWROFF_TIMEOUT); // Wait 100 ms for discharge
	DebugSendData("Usr Disable",11);


}
/* --------------- End of the ULT SENSOR DISABLE ---------------- */

/* ------- ULT SENSOR EMIT -------------------- */
void UsrEmit()
{
	// Emit Process
	HAL_GPIO_WritePin(ULT_EMIT_GPIO_Port, ULT_EMIT_Pin, GPIO_PIN_SET); // Set emit pin to high
	DelayUs(500); // 0.5 ms high is necessary for emit
	HAL_GPIO_WritePin(ULT_EMIT_GPIO_Port, ULT_EMIT_Pin, GPIO_PIN_RESET); // Set emit pin to low



}
/* --------------- End of the ULT SENSOR EMIT ---------------- */

/* ------- ULT SENSOR MEASURE -------------------- */
void UsrMeasure()
{
	DebugSendData("Usr Measure",11);
	uint8_t arrayCnt = 0;

	for(uint16_t cnt = 0; cnt < usrParameters.measureRepCount; cnt++)
	{
		usrParameters.measurementValue = 0; //Set the measurement value to 0
		usrParameters.usrReadyRead = 0; //Set the read mode to disable

		UsrEmit(); //Emit the Ultrasonic Sensor

		usrParameters.readDistance = NEAR; //Set the read distance mode to NEAR
		DelayUs(USR_NEAR_MEAS_TIMEOUT); //Wait 2 ms for the near distance measurement (35 cm)

		usrParameters.readDistance = FAR; //Set the read distance mode to FAR
		usrParameters.usrReadyRead = 1; //Set the read mode to enable

		DelayUs(USR_FAR_MEAS_TIMEOUT); //Wait 28 ms for the far distance measurement

		usrParameters.usrReadyRead = 0; //Set the read mode to disable

		usrParameters.usrMeasArray[arrayCnt] = usrParameters.measurementValue + USR_NEAR_MEAS_TIMEOUT;
		arrayCnt ++;
	}


}
/* --------------- End of the ULT SENSOR MEASURE ---------------- */

/************************ (C) COPYRIGHT cloud *****END OF FILE****/
