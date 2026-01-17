/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"

#include "app_tasks.h"

#include "battery_monitor.h"
#include "ppg_processing.h"

#include "queue.h"
#include "semphr.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for H2_SPO2_calc_ta */
osThreadId_t H2_SPO2_calc_taHandle;
uint32_t H2_SPO2_calc_taskBuffer[1024];
osStaticThreadDef_t H2_SPO2_calc_taskControlBlock;
const osThreadAttr_t H2_SPO2_calc_ta_attributes = {
  .name = "H2_SPO2_calc_ta",
  .cb_mem = &H2_SPO2_calc_taskControlBlock,
  .cb_size = sizeof(H2_SPO2_calc_taskControlBlock),
  .stack_mem = &H2_SPO2_calc_taskBuffer[0],
  .stack_size = sizeof(H2_SPO2_calc_taskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for Battery_monitor */
osThreadId_t Battery_monitorHandle;
uint32_t Battery_monitorBuffer[128];
osStaticThreadDef_t Battery_monitorControlBlock;
const osThreadAttr_t Battery_monitor_attributes = {
  .name = "Battery_monitor",
  .cb_mem = &Battery_monitorControlBlock,
  .cb_size = sizeof(Battery_monitorControlBlock),
  .stack_mem = &Battery_monitorBuffer[0],
  .stack_size = sizeof(Battery_monitorBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for MQTT_publisher */
osThreadId_t MQTT_publisherHandle;
uint32_t MQTT_publisherBuffer[1024];
osStaticThreadDef_t MQTT_publisherControlBlock;
const osThreadAttr_t MQTT_publisher_attributes = {
  .name = "MQTT_publisher",
  .cb_mem = &MQTT_publisherControlBlock,
  .cb_size = sizeof(MQTT_publisherControlBlock),
  .stack_mem = &MQTT_publisherBuffer[0],
  .stack_size = sizeof(MQTT_publisherBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for Datalogger */
osThreadId_t DataloggerHandle;
uint32_t DataloggerBuffer[128];
osStaticThreadDef_t DataloggerControlBlock;
const osThreadAttr_t Datalogger_attributes = {
  .name = "Datalogger",
  .cb_mem = &DataloggerControlBlock,
  .cb_size = sizeof(DataloggerControlBlock),
  .stack_mem = &DataloggerBuffer[0],
  .stack_size = sizeof(DataloggerBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for Display_data */
osThreadId_t Display_dataHandle;
uint32_t Display_dataBuffer[512];
osStaticThreadDef_t Display_dataControlBlock;
const osThreadAttr_t Display_data_attributes = {
  .name = "Display_data",
  .cb_mem = &Display_dataControlBlock,
  .cb_size = sizeof(Display_dataControlBlock),
  .stack_mem = &Display_dataBuffer[0],
  .stack_size = sizeof(Display_dataBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for Start_measure */
osSemaphoreId_t Start_measureHandle;
osStaticSemaphoreDef_t Start_measureControlBlock;
const osSemaphoreAttr_t Start_measure_attributes = {
  .name = "Start_measure",
  .cb_mem = &Start_measureControlBlock,
  .cb_size = sizeof(Start_measureControlBlock),
};

/* Definitions for push_button */
osEventFlagsId_t push_buttonHandle;
osStaticEventGroupDef_t push_buttonControlBlock;
const osEventFlagsAttr_t push_button_attributes = {
  .name = "push_button",
  .cb_mem = &push_buttonControlBlock,
  .cb_size = sizeof(push_buttonControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM9_Init(void);

void StartDefaultTask(void *argument);
void Start_HR_SPO2_task(void *argument);
void Start_Battery_monitor(void *argument);
void Start_publisher(void *argument);
void Start_Datalogging(void *argument);
void Start_Displaying(void *argument);

/* USER CODE BEGIN 0 */

void Start_HR_SPO2_task(void *argument)
{
    for (;;)
    {
        PPG_ProcessStep();
        osDelay(1);
    }
}

void Start_Battery_monitor(void *argument)
{
    BatteryMonitor_Init(&hadc1, GPIOA, Battery_Alarm_Led_Pin);

    for (;;)
    {
        BatteryMonitor_Update();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Start_publisher(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void Start_Displaying(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void Start_Datalogging(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}
/**
 * @brief  GPIO EXTI callback.
 *
 * This function is called by the HAL when an external interrupt
 * occurs on a configured GPIO pin and transmits a character through 
 * UART2 interface (SIMULATION only).
 *
 * @param[in] GPIO_Pin  Specifies the pins connected to EXTI line.
 *
 * @note This function is a user implementation of the weak
 *       HAL_GPIO_EXTI_Callback() provided by the STM32 HAL.
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == Start_measure_button_Pin)
    {
        uint8_t c = 'C';
        HAL_UART_Transmit(&huart2, &c, 1, HAL_MAX_DELAY);
        PPG_Start();
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_USART2_UART_Init();
  MX_TIM9_Init();

  HAL_TIM_Base_Start_IT(&htim9);

#ifdef USE_SIMULATION
  PPG_Init(0, 0);
#else
  PPG_Init(&hadc1, &hadc1);
#endif

  osKernelInitialize();

  Start_measureHandle = osSemaphoreNew(1, 0, &Start_measure_attributes);

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  H2_SPO2_calc_taHandle = osThreadNew(Start_HR_SPO2_task, NULL, &H2_SPO2_calc_ta_attributes);
  Battery_monitorHandle = osThreadNew(Start_Battery_monitor, NULL, &Battery_monitor_attributes);
  MQTT_publisherHandle = osThreadNew(Start_publisher, NULL, &MQTT_publisher_attributes);
  DataloggerHandle = osThreadNew(Start_Datalogging, NULL, &Datalogger_attributes);
  Display_dataHandle = osThreadNew(Start_Displaying, NULL, &Display_data_attributes);

  push_buttonHandle = osEventFlagsNew(&push_button_attributes);

  osKernelStart();

  while (1) {}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK
                             | RCC_CLOCKTYPE_SYSCLK
                             | RCC_CLOCKTYPE_PCLK1
                             | RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 1599;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 99;
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
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA,
                    IR_PWM_LED_Pin | Red_PWM_LED_Pin | Battery_Alarm_Led_Pin,
                    GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = Start_measure_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Start_measure_button_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IR_PWM_LED_Pin
                      | Red_PWM_LED_Pin
                      | Battery_Alarm_Led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief Period elapsed callback in non blocking mode
  * @note  Called when TIM9 interrupt occurs
  * @param htim TIM handle
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance != TIM9)
    return;

#ifdef USE_SIMULATION
  if (PPG_IsRunning())
  {
    uint8_t req = 'R';
    HAL_UART_Transmit_IT(&huart2, &req, 1);
  }
#endif
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file Pointer to source file name
  * @param  line Assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
