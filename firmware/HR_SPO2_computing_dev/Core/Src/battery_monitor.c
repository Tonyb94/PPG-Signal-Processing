/**
 ******************************************************************************
 * @file    battery_monitor.c
 * @brief   Battery monitor module implementation.
 ******************************************************************************
 */


#include "battery_monitor.h"

static ADC_HandleTypeDef *hadc_private;
static GPIO_TypeDef *led_port_private;
static uint16_t led_pin_private;

#define BATTERY_THRESHOLD 3354  // to configure better in future versions



void BatteryMonitor_Init(ADC_HandleTypeDef *adc, GPIO_TypeDef *ledPort, uint16_t ledPin)
{
    hadc_private = adc;
    led_port_private = ledPort;
    led_pin_private = ledPin;
}



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