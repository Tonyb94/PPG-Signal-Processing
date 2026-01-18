/**
 ******************************************************************************
 * @file    battery_monitor.c
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
 *
 ******************************************************************************
 */

#include "battery_monitor.h"

static ADC_HandleTypeDef *hadc_private;
static GPIO_TypeDef *led_port_private;
static uint16_t led_pin_private;

#define BATTERY_THRESHOLD 3354  // to configure better in future versions

/**
 * @brief  Initializes the HW to perform Battery monitoring.
 *
 * @param[in]   adc        ADC_HandleTypeDef pointer descriptor to select ADC.
 * @param[in]   ledPort    GPIO_TypeDef pointer descriptor to select port for alarm
 * @param[in]   ledPin     uint16_t to select the pin on MCU to perform alarm
 *
 * @return void
 */


void BatteryMonitor_Init(ADC_HandleTypeDef *adc, GPIO_TypeDef *ledPort, uint16_t ledPin)
{
    hadc_private = adc;
    led_port_private = ledPort;
    led_pin_private = ledPin;
}

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


void BatteryMonitor_Update(void)
{
    uint32_t adcValue = 0;

    HAL_ADC_Start(hadc_private);

    if (HAL_ADC_PollForConversion(hadc_private, HAL_MAX_DELAY) == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(hadc_private);

        if (adcValue < BATTERY_THRESHOLD)
            HAL_GPIO_WritePin(led_port_private, led_pin_private, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(led_port_private, led_pin_private, GPIO_PIN_RESET);
    }

    HAL_ADC_Stop(hadc_private);
}

/*End of file*/