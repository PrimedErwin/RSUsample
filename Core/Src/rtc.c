/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include "time.h"
//#include <stm32f4xx_hal_rtc.h>
//#include <stdint.h>
RTC_DateTypeDef sdate;
RTC_TimeTypeDef stime;
time_t count;
tm* currentTime = new tm;
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 124;
  hrtc.Init.SynchPrediv = 3999;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x12;
  sTime.Minutes = 0x22;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
  sDate.Month = RTC_MONTH_AUGUST;
  sDate.Date = 0x1;
  sDate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV16;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//**************************************//
//RTC basic functions
//**************************************//

uint32_t _u8_4_to_32(uint8_t* _4byte)
{
	return (uint32_t)(_4byte[0] << 24 | _4byte[1] << 16
		| _4byte[2] << 8 | _4byte[3]);
}

void _u32_to_u8_4(uint32_t _int32b, uint8_t* _4byte)
{
	_4byte[0] = (uint8_t)(_int32b >> 24);
	_4byte[1] = (uint8_t)(_int32b << 8 >> 24);
	_4byte[2] = (uint8_t)(_int32b << 16 >> 24);
	_4byte[3] = (uint8_t)(_int32b << 24 >> 24);
}

void unixTime_setRTC(uint8_t* unix, RTC_HandleTypeDef* hrtc)
{
	count = _u8_4_to_32(unix);

	currentTime = localtime(&count);
	sdate.Year = currentTime->tm_year - 100;
	sdate.Month = currentTime->tm_mon + 1;
	sdate.Date = currentTime->tm_mday;
	stime.Hours = currentTime->tm_hour + 8;
	stime.Minutes = currentTime->tm_min;
	stime.Seconds = currentTime->tm_sec;

	HAL_RTC_SetDate(hrtc, &sdate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(hrtc, &stime, RTC_FORMAT_BIN);
}

void unixTime_setFrame(uint8_t* unix, RTC_HandleTypeDef* hrtc)
{
	HAL_RTC_GetTime(hrtc, &stime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &sdate, RTC_FORMAT_BIN);

	currentTime->tm_year = sdate.Year + 100;
	currentTime->tm_mon = sdate.Month - 1;
	currentTime->tm_mday = sdate.Date;
	currentTime->tm_hour = stime.Hours - 8;
	currentTime->tm_min = stime.Minutes;
	currentTime->tm_sec = stime.Seconds;

	count = mktime(currentTime);
	_u32_to_u8_4(count, unix);
}
/* USER CODE END 1 */
