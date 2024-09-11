/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "tim.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define ToggleLD1() HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
#define ToggleLD2() HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
#define ToggleLD3() HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
#define TC32169_BOOT() HAL_GPIO_WritePin(TC_CE_GPIO_Port, TC_CE_Pin, GPIO_PIN_RESET);\
delay_us(888);\
HAL_GPIO_WritePin(TC_CE_GPIO_Port, TC_CE_Pin, GPIO_PIN_SET);\
delay_us(888);
#define TC_Tx() HAL_GPIO_WritePin(TC_TXRX_GPIO_Port, TC_TXRX_Pin, GPIO_PIN_SET);
#define TC_Rx() HAL_GPIO_WritePin(TC_TXRX_GPIO_Port, TC_TXRX_Pin, GPIO_PIN_RESET);
#define SwitchTx() HAL_GPIO_WritePin(VCTL_GPIO_Port, VCTL_Pin, GPIO_PIN_RESET);\
SwitchTx_USED = 1;
#define SwitchRx() HAL_GPIO_WritePin(VCTL_GPIO_Port, VCTL_Pin, GPIO_PIN_SET);\
SwitchRx_USED = 1;
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

