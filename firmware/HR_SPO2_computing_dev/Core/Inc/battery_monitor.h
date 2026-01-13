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

void BatteryMonitor_Init(ADC_HandleTypeDef *adc, GPIO_TypeDef *ledPort, uint16_t ledPin);
void BatteryMonitor_Update(void);

#endif
