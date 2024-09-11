/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

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

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define K2_Pin GPIO_PIN_13
#define K2_GPIO_Port GPIOC
#define K2_EXTI_IRQn EXTI15_10_IRQn
#define SPI2_NSS_Pin GPIO_PIN_3
#define SPI2_NSS_GPIO_Port GPIOA
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define From_103_Pin GPIO_PIN_0
#define From_103_GPIO_Port GPIOB
#define From_103_EXTI_IRQn EXTI0_IRQn
#define VCTL_Pin GPIO_PIN_12
#define VCTL_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_9
#define LD2_GPIO_Port GPIOD
#define LD1_Pin GPIO_PIN_11
#define LD1_GPIO_Port GPIOD
#define TC_IRQ_Pin GPIO_PIN_10
#define TC_IRQ_GPIO_Port GPIOC
#define TC_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define TC_CE_Pin GPIO_PIN_11
#define TC_CE_GPIO_Port GPIOC
#define TC_TXRX_Pin GPIO_PIN_12
#define TC_TXRX_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
