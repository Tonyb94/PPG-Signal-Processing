/**
 ******************************************************************************
 * @file    battery_monitor.h
 * @author  A. Bellina
 * @brief   Battery monitoring module.
 *
 * @details
 * This module provides APIs to measure battery level using the ADC and to
 * evaluate thresholds for battery alarms. It is meant to be used inside a
 * FreeRTOS task but can also be called from bare-metal code.
 *
 * Features:
 * - ADC acquisition wrapper
 * - Configurable voltage thresholds
 * - Integration with GPIO alarm LED
 *
 * Dependencies:
 * - HAL ADC driver
 * - HAL GPIO driver
 *
 * Version:
 * - v1.0 — Initial version
 ******************************************************************************
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/**
 * @brief  Initializes the HW to perform Battery monitoring.
 *
 * @param[in]   adc        ADC_HandleTypeDef pointer descriptor to select ADC.
 * @param[in]   ledPort    GPIO_TypeDef pointer descriptor to select port for alarm
 * @param[in]   ledPin     uint16_t to select the pin on MCU to perform alarm
 *
 * @return void
 */

void BatteryMonitor_Init(ADC_HandleTypeDef *adc, GPIO_TypeDef *ledPort, uint16_t ledPin);

/**
 * @brief  Algorithm to check the voltage level of Battery with HW configured by calling BatteryMonitor() and manage alarm output.
 *
 * @param[in]  void 
 *
 * @pre     BatteryMonitor_Init() must be called before this function.
 * @post    Turns on the led when Battery level voltage goes under the threshold.
 *
 *
 * @warning connect the HW with the right configured pins and ADC.  
 */

void BatteryMonitor_Update(void);

#endif
