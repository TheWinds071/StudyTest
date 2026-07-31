/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define Button_DOWN_Pin GPIO_PIN_4
#define Button_DOWN_GPIO_Port GPIOE
#define Button_IN_Pin GPIO_PIN_5
#define Button_IN_GPIO_Port GPIOE
#define Button_UP_Pin GPIO_PIN_6
#define Button_UP_GPIO_Port GPIOE
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_0
#define LED_R_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOC
#define LED_B_Pin GPIO_PIN_2
#define LED_B_GPIO_Port GPIOC
#define ENCODERL2_Pin GPIO_PIN_0
#define ENCODERL2_GPIO_Port GPIOA
#define ENCODERL1_Pin GPIO_PIN_1
#define ENCODERL1_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define Servo_TX_Pin GPIO_PIN_8
#define Servo_TX_GPIO_Port GPIOE
#define MOTORL1_Pin GPIO_PIN_9
#define MOTORL1_GPIO_Port GPIOE
#define MOTORL2_Pin GPIO_PIN_11
#define MOTORL2_GPIO_Port GPIOE
#define MOTORR1_Pin GPIO_PIN_13
#define MOTORR1_GPIO_Port GPIOE
#define MOTORR2_Pin GPIO_PIN_14
#define MOTORR2_GPIO_Port GPIOE
#define ENCODERR2_Pin GPIO_PIN_6
#define ENCODERR2_GPIO_Port GPIOC
#define ENCODERR1_Pin GPIO_PIN_7
#define ENCODERR1_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
