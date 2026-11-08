/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user.h"
#include "board_i2c.h"
#include "bsp_flash.h"
#include "tm1650.h"
#include "drv_18b20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim12;

UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_I2C3_Init(void);
static void MX_SPI1_Init(void);
static void MX_UART4_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM12_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM8_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern FlashParam_TypeDef FlashParam_Struct;
extern LED_Para_TypeDef LED_Para_Struct ; 
extern _18B20_Config_TypeDef _18B20_Config_Struct_Temp;
uint16_t time_count1=0,time_count2=0,time_cnt=0,time_cnt1=0;

uint8_t  cnt1 = 0,cnt2 = 0,cnt3 = 0,cnt_t = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_SPI1_Init();
  MX_UART4_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  MX_TIM12_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  Comm_Init();
  IIC_Init();
  Parameter_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

		Comm_Function_Handler();
	  LED_State_Updata();
		LED_OTP_Handler();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		HAL_Delay(100);
//		HAL_GPIO_TogglePin(LED_PA3_GPIO_Port,LED_PA3_Pin);
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 840-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 420-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 420-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 420-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 8400-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 420-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 840-1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 1680;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 1000-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 16800-1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 20000-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 0;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 1680-1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM12 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM12_Init(void)
{

  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 0;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 840-1;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 250000;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, RS485_EN_Pin|Temp_DQ_Pin|AR_PWM_Pin|AG_PWM_Pin
                          |AB_PWM_Pin|AW_PWM_Pin|MR_PWM_Pin|MG_PWM_Pin
                          |MB_PWM_Pin|MW1_PWM_Pin|MW2_PWM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PA3_GPIO_Port, LED_PA3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SPI1_CS_Pin|Reserve_PWM_Pin|VCSEL_TRIG_OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LIGHT_FAN_Pin|IIC3_LDAC_Pin|IIC2_LDAC_Pin|IIC1_LDAC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, CR_PWM_Pin|CG_PWM_Pin|CB1_PWM_Pin|CW1_PWM_Pin
                          |CW2_PWM_Pin|CA_PWM_Pin|CC_PWM_Pin|CB2_PWM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CAM_TRIG_OUT_Pin|CAM_FOCUS_Pin|DISPLAY_SDA_Pin|DISPLAY_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RS485_EN_Pin Temp_DQ_Pin */
  GPIO_InitStruct.Pin = RS485_EN_Pin|Temp_DQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : TRIG_MCU_Pin */
  GPIO_InitStruct.Pin = TRIG_MCU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TRIG_MCU_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_PA3_Pin CAM_TRIG_OUT_Pin CAM_FOCUS_Pin DISPLAY_SDA_Pin
                           DISPLAY_SCL_Pin */
  GPIO_InitStruct.Pin = LED_PA3_Pin|CAM_TRIG_OUT_Pin|CAM_FOCUS_Pin|DISPLAY_SDA_Pin
                          |DISPLAY_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_CS_Pin Reserve_PWM_Pin VCSEL_TRIG_OUT_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin|Reserve_PWM_Pin|VCSEL_TRIG_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : AR_PWM_Pin AG_PWM_Pin AB_PWM_Pin AW_PWM_Pin
                           MR_PWM_Pin MG_PWM_Pin MB_PWM_Pin MW1_PWM_Pin
                           MW2_PWM_Pin */
  GPIO_InitStruct.Pin = AR_PWM_Pin|AG_PWM_Pin|AB_PWM_Pin|AW_PWM_Pin
                          |MR_PWM_Pin|MG_PWM_Pin|MB_PWM_Pin|MW1_PWM_Pin
                          |MW2_PWM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LIGHT_FAN_Pin IIC3_LDAC_Pin IIC2_LDAC_Pin IIC1_LDAC_Pin */
  GPIO_InitStruct.Pin = LIGHT_FAN_Pin|IIC3_LDAC_Pin|IIC2_LDAC_Pin|IIC1_LDAC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CR_PWM_Pin CG_PWM_Pin CB1_PWM_Pin CW1_PWM_Pin
                           CW2_PWM_Pin CA_PWM_Pin CC_PWM_Pin CB2_PWM_Pin */
  GPIO_InitStruct.Pin = CR_PWM_Pin|CG_PWM_Pin|CB1_PWM_Pin|CW1_PWM_Pin
                          |CW2_PWM_Pin|CA_PWM_Pin|CC_PWM_Pin|CB2_PWM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin & TRIG_MCU_Pin)
	{
     __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);         
		 __HAL_TIM_SetCounter(&htim6, 0);        /*开启定时器6，5us后中断检测触发电平，防抖动*/
					HAL_TIM_Base_Start_IT(&htim6);
		__HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);	
		  __HAL_TIM_SetCounter(&htim9, 0);       /*开启定时器9，2S后若无新的触发则关闭外部中断*/
		HAL_TIM_Base_Start_IT(&htim9);
	} 
	
}

/*定时器中断配置*/
/*TIM2  重光照模式下R通道输出占空比控制*/
/*TIM3  重光照模式下G通道输出占空比控制*/
/*TIM4  重光照模式下B通道输出占空比控制*/
/*TIM5  重光照模式下PWM频率控制 100us中断对应频率10kHz*/
/*TIM6  外部触发防抖检测 触发响应5us后检测触发电平*/
/*TIM7  触发脉宽控制，基础定时10us*/
/*TIM8  单反相机对焦脉宽，基础定时10ms*/
/*TIM9  触发间隔时间检测，定时2s，2s后若无新触发，则关闭外部中断*/
/*TIM10  相机触发延时控制，基础定时10us*/
/*TIM12  相机触发脉宽控制，基础定时10us*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)                                                       /*重光照模式（或重光照调试模式）下R通道输出占空比控制*/
	{

			LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_OFF ;
			 HAL_TIM_Base_Stop_IT(&htim2);
			__HAL_TIM_SetCounter(&htim2, 0);

	}
	else if(htim == &htim3)                                                 /*重光照模式（或重光照调试模式）下G通道输出占空比控制*/
	{

			LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_OFF ;
					 HAL_TIM_Base_Stop_IT(&htim3);
			__HAL_TIM_SetCounter(&htim3, 0);

	}
	else if(htim == &htim4)                                                 /*重光照模式（或重光照调试模式）下B通道输出占空比控制*/
	{

			LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_OFF ;
			HAL_TIM_Base_Stop_IT(&htim4);
			__HAL_TIM_SetCounter(&htim4, 0);

	}
	else if(htim == &htim5)                                                 /*重光照模式（或重光照调试模式）下PWM频率控制*/
	{
			  HAL_TIM_Base_Stop_IT(&htim2);
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);         
		__HAL_TIM_SetCounter(&htim2, 0);       /*关闭定时器2，停止占空比输出*/
						  
			
			HAL_TIM_Base_Stop_IT(&htim3);
		__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);         
		__HAL_TIM_SetCounter(&htim3, 0);       /*关闭定时器3，停止占空比输出*/
						
			HAL_TIM_Base_Stop_IT(&htim4);
		__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);         
		__HAL_TIM_SetCounter(&htim4, 0);       /*关闭定时器4，停止占空比输出*/

		if(LED_Para_Struct.Relighting_parameter.Dr!=0) 		
		{
			LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_ON;
			
			htim2.Init.Period = LED_Para_Struct.Relighting_parameter.Dr *84-1;
      __HAL_TIM_SetAutoreload(&htim2,htim2.Init.Period);    /*设置占空比*/
			HAL_TIM_Base_Start_IT(&htim2);		/*开启定时器2，进行占空比输出*/
		}
		else LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_OFF;

		if(LED_Para_Struct.Relighting_parameter.Dg!=0) 		
		{
			LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_ON;
			
			htim3.Init.Period = LED_Para_Struct.Relighting_parameter.Dg *84-1; 
      __HAL_TIM_SetAutoreload(&htim3,htim3.Init.Period);    /*设置占空比*/
			HAL_TIM_Base_Start_IT(&htim3);		/*开启定时器3，进行占空比输出*/
		}
		else LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_OFF;
	
		if(LED_Para_Struct.Relighting_parameter.Db!=0) 		
		{
			LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_ON;
			
			htim4.Init.Period = LED_Para_Struct.Relighting_parameter.Db *84-1; 
      __HAL_TIM_SetAutoreload(&htim4,htim4.Init.Period);    /*设置占空比*/
			HAL_TIM_Base_Start_IT(&htim4);		/*开启定时器4，进行占空比输出*/
		}
		else LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_OFF;		
	}

	
	else if(htim == &htim6)              /*5us后检测触发电平*/	
	{
		LED_Para_Struct.Trig_Level = HAL_GPIO_ReadPin(TRIG_MCU_GPIO_Port,TRIG_MCU_Pin);
		if(LED_Para_Struct.Trig_Level)     /*触发有效*/
		{					
			  LED_Para_Struct.Trig_Cnt++;	
			if(LED_Para_Struct.Trig_Cnt == LED_Para_Struct.Data_Cycle_quantity)
			{
				LED_Para_Struct.Trig_Cnt = 1;
			}				
        LED_Para_Struct.Camera_Trig_cnt	++;		
			  LED_Para_Struct.Idle_Flag = 0; 
//			LED_Para_Struct.Start_LED_TRIG_ON = 1;
			
				if(LED_Para_Struct.Mode == Material_solve_MODE)                                      /*材质解算模式下，赋值相应参数*/
				{	
					if(LED_Para_Struct.Material_solve_parameter.R_Pulse[LED_Para_Struct.Trig_Cnt-1]!=0)	
					{
						LED_Para_Struct.Material_solve_parameter.Switch_Pulse_R = LED_Para_Struct.Material_solve_parameter.R_Pulse[LED_Para_Struct.Trig_Cnt-1];	
            LED_Para_Struct.Material_solve_parameter.R_Pulse_Flag = 1;						
					}
					else 
					{
						LED_Para_Struct.Material_solve_parameter.Switch_Pulse_R = 0;
						LED_Para_Struct.Material_solve_parameter.R_Pulse_Flag = 0;
					}
					if(LED_Para_Struct.Material_solve_parameter.G_Pulse[LED_Para_Struct.Trig_Cnt-1]!=0)
					{
						LED_Para_Struct.Material_solve_parameter.Switch_Pulse_G = LED_Para_Struct.Material_solve_parameter.G_Pulse[LED_Para_Struct.Trig_Cnt-1];	
            LED_Para_Struct.Material_solve_parameter.G_Pulse_Flag = 1;						
					}
					else 
					{
						LED_Para_Struct.Material_solve_parameter.Switch_Pulse_G = 0;	
						LED_Para_Struct.Material_solve_parameter.G_Pulse_Flag = 0;						
					}
					if(LED_Para_Struct.Material_solve_parameter.B_Pulse[LED_Para_Struct.Trig_Cnt-1]!=0)
					{
						LED_Para_Struct.Material_solve_parameter.Switch_Pulse_B = LED_Para_Struct.Material_solve_parameter.B_Pulse[LED_Para_Struct.Trig_Cnt-1];	
            LED_Para_Struct.Material_solve_parameter.B_Pulse_Flag = 1;												
					}
					else 
					{
						LED_Para_Struct.Material_solve_parameter.Switch_Pulse_B = 0;
						LED_Para_Struct.Material_solve_parameter.B_Pulse_Flag = 0;						
					}
				}
				else if(LED_Para_Struct.Mode == OLAT_MODE)                                          /*OLAT模式下，赋值相应参数*/
				{
					if(LED_Para_Struct.OLAT_parameter.W_Pulse[LED_Para_Struct.Trig_Cnt-1]!=0)
					{	
						LED_Para_Struct.OLAT_parameter.Switch_Pulse_W = LED_Para_Struct.OLAT_parameter.W_Pulse[LED_Para_Struct.Trig_Cnt-1];					
					}
					else LED_Para_Struct.OLAT_parameter.Switch_Pulse_W = 0;
				}
				
				
				else if(LED_Para_Struct.Mode == Relighting_MODE)                                      /*重光照模式下，赋值相应参数*/
				{
					if(LED_Para_Struct.Relighting_parameter.Pulse[LED_Para_Struct.Trig_Cnt-1]!=0)
					{
						LED_Para_Struct.Relighting_parameter.Switch_Pulse = LED_Para_Struct.Relighting_parameter.Pulse[LED_Para_Struct.Trig_Cnt-1];
						
						           /*色坐标解析*/

						LED_Para_Struct.Relighting_parameter.Coordinate_X	= LED_Para_Struct.Relighting_parameter.Color_Coordinate_X[LED_Para_Struct.Trig_Cnt-1];
						LED_Para_Struct.Relighting_parameter.Coordinate_Y	= LED_Para_Struct.Relighting_parameter.Color_Coordinate_Y[LED_Para_Struct.Trig_Cnt-1];
					}
					else LED_Para_Struct.Relighting_parameter.Switch_Pulse = 0;
				}
			
			
			/*相机类型为单反，则需先进行对焦*/
			if(LED_Para_Struct.Camera_Type)
			{	
					/*相机分频控制*/
				if(LED_Para_Struct.Camera_Trig_cnt == LED_Para_Struct.Camera_div_Frequency)
				{
					//LED_Para_Struct.Camera_Trig_cnt = 0;
					/*开启定时器8，进行相机对焦控制，基础步长10ms*/
					Camera_Focus_ON();
					__HAL_TIM_CLEAR_IT(&htim8, TIM_IT_UPDATE);	
					__HAL_TIM_SetCounter(&htim8, 0);      
					HAL_TIM_Base_Start_IT(&htim8);
				}
				else
				{
					LED_Para_Struct.Start_LED_TRIG_Flag = 1;
//				/*开启定时器7，进行光源触发脉宽控制，基础步长10us*/
//					__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);	
//					__HAL_TIM_SetCounter(&htim7, 0);      
//					HAL_TIM_Base_Start_IT(&htim7);
				}
			}	
			
			/*相机类型为工业相机，则直接进行光源触发控制和相机延迟触发控制*/
			else
			{
				if(LED_Para_Struct.Camera_Trig_cnt == LED_Para_Struct.Camera_div_Frequency)
				{
					LED_Para_Struct.Start_Camera_TRIG_Flag = 1;
				}
				else;
				
				LED_Para_Struct.Start_LED_TRIG_Flag = 1;	
				

			}
			


    /*实际触发次数和设置触发次数相同时，关闭外部中断，防止误触发*/
			if(LED_Para_Struct.Trig_Cnt == LED_Para_Struct.Trig_Cnt_Set)
			{
	      HAL_NVIC_DisableIRQ(EXTI0_IRQn);    //关闭外部中断
			}	
		}
					
		HAL_TIM_Base_Stop_IT(&htim6);       //关定时器
	}	
	
	else if(htim == &htim7)             /*光源触发脉宽控制，基础步长10us*/
	{	
		if(LED_Para_Struct.Mode == Material_solve_MODE)                                      /*材质解算模式下，进行脉宽控制*/
		{
			if(LED_Para_Struct.Material_solve_parameter.R_Pulse_Flag)
			{
				LED_Para_Struct.Material_solve_parameter.R_Pulse_cnt++;
				if(LED_Para_Struct.Material_solve_parameter.R_Pulse_cnt == LED_Para_Struct.Material_solve_parameter.Switch_Pulse_R )
				{
					LED_Para_Struct.MR_LED_State = LIGHT_OFF;	
					LED_Para_Struct.Material_solve_parameter.R_Pulse_cnt = 0;							
          LED_Para_Struct.Material_solve_parameter.R_Pulse_Flag = 0;
				}	
			}
			
			if(LED_Para_Struct.Material_solve_parameter.G_Pulse_Flag)
			{
				LED_Para_Struct.Material_solve_parameter.G_Pulse_cnt++;
				if(LED_Para_Struct.Material_solve_parameter.G_Pulse_cnt == LED_Para_Struct.Material_solve_parameter.Switch_Pulse_G )
				{
					LED_Para_Struct.MG_LED_State = LIGHT_OFF;	
					LED_Para_Struct.Material_solve_parameter.G_Pulse_cnt = 0;	
					LED_Para_Struct.Material_solve_parameter.G_Pulse_Flag = 0;
				}	
			}
			
			if(LED_Para_Struct.Material_solve_parameter.B_Pulse_Flag)
			{				
				LED_Para_Struct.Material_solve_parameter.B_Pulse_cnt++;
				if(LED_Para_Struct.Material_solve_parameter.B_Pulse_cnt == LED_Para_Struct.Material_solve_parameter.Switch_Pulse_B )
				{
					LED_Para_Struct.MB_LED_State = LIGHT_OFF;	
					LED_Para_Struct.Material_solve_parameter.B_Pulse_cnt = 0;		
          LED_Para_Struct.Material_solve_parameter.B_Pulse_Flag = 0;					
				}	
			}	
			
			/*RGB脉宽时间到则关闭脉宽控制定时器*/
			if((LED_Para_Struct.Material_solve_parameter.R_Pulse_Flag ==0)&&
			   (LED_Para_Struct.Material_solve_parameter.G_Pulse_Flag ==0)&&
			   (LED_Para_Struct.Material_solve_parameter.B_Pulse_Flag ==0))
			{
				HAL_TIM_Base_Stop_IT(&htim7);       //关闭脉宽控制定时器	
			}
		}			

		else if(LED_Para_Struct.Mode == OLAT_MODE)                                          /*OLAT模式下，进行脉宽控制*/	
		{
				LED_Para_Struct.OLAT_parameter.W_Pulse_cnt++;
				if(LED_Para_Struct.OLAT_parameter.W_Pulse_cnt == LED_Para_Struct.OLAT_parameter.Switch_Pulse_W )
				{
					LED_Para_Struct.MW_LED_State = LIGHT_OFF;	
					LED_Para_Struct.OLAT_parameter.W_Pulse_cnt = 0;		
					HAL_TIM_Base_Stop_IT(&htim7);       //关闭脉宽控制定时器	
				}		
		}	
	
			else if(LED_Para_Struct.Mode == Relighting_MODE)	                                  /*重光照模式下，进行脉宽控制*/
			{
				LED_Para_Struct.Relighting_parameter.Pulse_cnt++;
				if(LED_Para_Struct.Relighting_parameter.Pulse_cnt == LED_Para_Struct.Relighting_parameter.Switch_Pulse)
				{
					LED_Para_Struct.Relighting_State_LED = LIGHT_OFF;	
					LED_Para_Struct.Relighting_parameter.Pulse_cnt = 0;	
					LED_Para_Struct.Start_LED_TRIG_ON = 0;
				
					/*重光照脉宽控制时间到，关闭占空比及频率控制定时器*/
					HAL_TIM_Base_Stop_IT(&htim2);       //关定时器2
					HAL_TIM_Base_Stop_IT(&htim3);       //关定时器3
					HAL_TIM_Base_Stop_IT(&htim4);       //关定时器4
					HAL_TIM_Base_Stop_IT(&htim5);       //关定时器5
				
					HAL_TIM_Base_Stop_IT(&htim7);       //关闭脉宽控制定时器7
				}
			}
					  
	}
	else if(htim == &htim8)             /*单反相机对焦10ms*/
	{
      Camera_Focus_OFF();
		 LED_Para_Struct.Start_LED_TRIG_Flag = 1;
		 LED_Para_Struct.Start_Camera_TRIG_Flag = 1;  /*单反相机对焦完成后光源相机有效触发*/
//		/*开启定时器7，进行光源触发脉宽控制，基础步长10us*/
//		__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);	
//		__HAL_TIM_SetCounter(&htim7, 0);      
//		HAL_TIM_Base_Start_IT(&htim7);
		
		/*相机触发延时不为0，进行相机触发延迟控制*/
//		if(LED_Para_Struct.Camera_Trig_Delay != 0)
//		{
//			/*开启定时器10，进行相机触发延迟控制，基础步长10us*/
//			__HAL_TIM_CLEAR_IT(&htim10, TIM_IT_UPDATE);	
//			__HAL_TIM_SetCounter(&htim10, 0);      
//			HAL_TIM_Base_Start_IT(&htim10);
//		}
//		/*相机触发延时为0，则直接进行相机触发及触发脉宽控制*/
//		else 
//		{
//			Camera_Trig_ON();
//			/*开启定时器12，进行相机触发脉宽控制，基础步长10us*/
//			__HAL_TIM_CLEAR_IT(&htim12, TIM_IT_UPDATE);	
//			__HAL_TIM_SetCounter(&htim12, 0);      
//			HAL_TIM_Base_Start_IT(&htim12);
//		}		
     HAL_TIM_Base_Stop_IT(&htim8);       //关定时器
	}
	else if(htim == &htim9)             /*2S后若无新的触发信号，则关闭外部中断*/
	{

			HAL_NVIC_DisableIRQ(EXTI0_IRQn);    //关闭外部中断
      LED_Para_Struct.Idle_Flag = 1 ;     //触发空闲
			LED_Para_Struct.Trig_Cnt = 0;       //触发次数清0
		  LED_Para_Struct.Camera_Trig_cnt = 0;
			HAL_TIM_Base_Stop_IT(&htim9);       //关定时器
	}
	else if(htim == &htim10)             /*相机延迟触发控制，基础定时10us*/
	{
		 LED_Para_Struct.Camera_Trig_Delay_cnt ++;  
		if(LED_Para_Struct.Camera_Trig_Delay_cnt == LED_Para_Struct.Camera_Trig_Delay)
		{
			LED_Para_Struct.Camera_Trig_Delay_cnt = 0;
			Camera_Trig_ON();
			
			/*开启定时器12，进行相机触发脉宽控制，基础步长20us*/
			__HAL_TIM_CLEAR_IT(&htim12, TIM_IT_UPDATE);	
			__HAL_TIM_SetCounter(&htim12, 0);      
			HAL_TIM_Base_Start_IT(&htim12);
			
			HAL_TIM_Base_Stop_IT(&htim10);       //关定时器
	
		}	
	}
		
	else if(htim == &htim12)             /*相机触发脉宽控制，基础定时10us*/
	{
		 LED_Para_Struct.Camera_Trig_Pulse_cnt ++;  
		if(LED_Para_Struct.Camera_Trig_Pulse_cnt == LED_Para_Struct.Camera_Trig_Pulse)
		{
			LED_Para_Struct.Camera_Trig_Pulse_cnt = 0;
			Camera_Trig_OFF();
			HAL_TIM_Base_Stop_IT(&htim12);       //关定时器
		}	
	}
	
	
}
	


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
