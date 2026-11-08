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
extern UART_HandleTypeDef huart4;
extern	I2C_HandleTypeDef hi2c1;
extern  I2C_HandleTypeDef hi2c2;
extern  I2C_HandleTypeDef hi2c3;
extern  TIM_HandleTypeDef htim1;
extern  TIM_HandleTypeDef htim2;
extern  TIM_HandleTypeDef htim3;
extern  TIM_HandleTypeDef htim4;
extern  TIM_HandleTypeDef htim5;
extern  TIM_HandleTypeDef htim6;
extern  TIM_HandleTypeDef htim7;
extern  TIM_HandleTypeDef htim8;
extern  TIM_HandleTypeDef htim9;
extern  TIM_HandleTypeDef htim10;
extern  TIM_HandleTypeDef htim12;
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
#define RS485_EN_Pin GPIO_PIN_2
#define RS485_EN_GPIO_Port GPIOE
#define Temp_DQ_Pin GPIO_PIN_3
#define Temp_DQ_GPIO_Port GPIOE
#define TRIG_MCU_Pin GPIO_PIN_0
#define TRIG_MCU_GPIO_Port GPIOC
#define TRIG_MCU_EXTI_IRQn EXTI0_IRQn
#define LED_PA3_Pin GPIO_PIN_3
#define LED_PA3_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOC
#define AR_PWM_Pin GPIO_PIN_7
#define AR_PWM_GPIO_Port GPIOE
#define AG_PWM_Pin GPIO_PIN_8
#define AG_PWM_GPIO_Port GPIOE
#define AB_PWM_Pin GPIO_PIN_9
#define AB_PWM_GPIO_Port GPIOE
#define AW_PWM_Pin GPIO_PIN_10
#define AW_PWM_GPIO_Port GPIOE
#define MR_PWM_Pin GPIO_PIN_11
#define MR_PWM_GPIO_Port GPIOE
#define MG_PWM_Pin GPIO_PIN_12
#define MG_PWM_GPIO_Port GPIOE
#define MB_PWM_Pin GPIO_PIN_13
#define MB_PWM_GPIO_Port GPIOE
#define MW1_PWM_Pin GPIO_PIN_14
#define MW1_PWM_GPIO_Port GPIOE
#define MW2_PWM_Pin GPIO_PIN_15
#define MW2_PWM_GPIO_Port GPIOE
#define LIGHT_FAN_Pin GPIO_PIN_12
#define LIGHT_FAN_GPIO_Port GPIOB
#define CR_PWM_Pin GPIO_PIN_8
#define CR_PWM_GPIO_Port GPIOD
#define CG_PWM_Pin GPIO_PIN_9
#define CG_PWM_GPIO_Port GPIOD
#define CB1_PWM_Pin GPIO_PIN_10
#define CB1_PWM_GPIO_Port GPIOD
#define CW1_PWM_Pin GPIO_PIN_11
#define CW1_PWM_GPIO_Port GPIOD
#define CW2_PWM_Pin GPIO_PIN_12
#define CW2_PWM_GPIO_Port GPIOD
#define CA_PWM_Pin GPIO_PIN_13
#define CA_PWM_GPIO_Port GPIOD
#define CC_PWM_Pin GPIO_PIN_14
#define CC_PWM_GPIO_Port GPIOD
#define CB2_PWM_Pin GPIO_PIN_15
#define CB2_PWM_GPIO_Port GPIOD
#define Reserve_PWM_Pin GPIO_PIN_6
#define Reserve_PWM_GPIO_Port GPIOC
#define VCSEL_TRIG_OUT_Pin GPIO_PIN_8
#define VCSEL_TRIG_OUT_GPIO_Port GPIOC
#define CAM_TRIG_OUT_Pin GPIO_PIN_9
#define CAM_TRIG_OUT_GPIO_Port GPIOA
#define CAM_FOCUS_Pin GPIO_PIN_10
#define CAM_FOCUS_GPIO_Port GPIOA
#define DISPLAY_SDA_Pin GPIO_PIN_11
#define DISPLAY_SDA_GPIO_Port GPIOA
#define DISPLAY_SCL_Pin GPIO_PIN_12
#define DISPLAY_SCL_GPIO_Port GPIOA
#define IIC3_LDAC_Pin GPIO_PIN_3
#define IIC3_LDAC_GPIO_Port GPIOB
#define IIC2_LDAC_Pin GPIO_PIN_4
#define IIC2_LDAC_GPIO_Port GPIOB
#define IIC1_LDAC_Pin GPIO_PIN_5
#define IIC1_LDAC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
