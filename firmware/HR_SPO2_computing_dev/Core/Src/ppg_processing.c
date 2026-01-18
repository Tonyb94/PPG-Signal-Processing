/**
 ******************************************************************************
 * @file    ppg_processing.c
 * @brief   PPG processing module implementation.
 ******************************************************************************
 */

#include "ppg_processing.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal_adc.h"
#include "FreeRTOS.h"
#include "queue.h"

/* ------------------------------------------------------------------------- */
/* Private state                                                             */
/* ------------------------------------------------------------------------- */

static ADC_HandleTypeDef *adc_red_h = NULL;
static ADC_HandleTypeDef *adc_ir_h  = NULL;
static QueueHandle_t ppgQueue = NULL;


static uint16_t filter_buffer[PPG_FILTER_WINDOW];
static uint8_t  filter_index = 0;
static uint8_t  filter_count = 0;

static volatile bool     ppg_running = false;
static volatile uint32_t ppg_sample_count = 0;

#ifdef USE_SIMULATION
volatile uint16_t adc_raw = 0;
volatile uint8_t  uart_ready = 0;
uint8_t usart_rx_buffer[BUFFER_SIZE];
#endif

/* ------------------------------------------------------------------------- */
/* Public debug variables (JScope)                                            */
/* ------------------------------------------------------------------------- */

volatile float    filtered_signal       = 0.0f;
volatile uint32_t ppg_red_filtered      = 0;
volatile uint32_t ppg_ir_filtered       = 0;
volatile float    ppg_heart_rate_bpm    = 0.0f;
volatile float    ppg_spo2_percent      = 0.0f;

volatile uint8_t dbg_start_called = 0;
volatile uint32_t uart_rx_cnt = 0;



/* ------------------------------------------------------------------------- */
/* Private helpers                                                           */
/* ------------------------------------------------------------------------- */

static uint16_t PPG_FilterMA(uint16_t sample)
{
#ifdef USE_SIMULATION
    PPG_PushSimulatedSample(sample);
#else
    adc_raw = sample;
#endif
    filter_buffer[filter_index] = adc_raw;
    filter_index = (filter_index + 1U) % PPG_FILTER_WINDOW;

    if (filter_count < PPG_FILTER_WINDOW)
        filter_count++;

    uint32_t sum = 0;
    for (uint8_t i = 0; i < filter_count; i++)
        sum += filter_buffer[i];

    return (uint16_t)(sum / filter_count);
}

/* ------------------------------------------------------------------------- */
/* Public API                                                                */
/* ------------------------------------------------------------------------- */

void PPG_Init(ADC_HandleTypeDef *adc_red, ADC_HandleTypeDef *adc_ir)
{
    adc_red_h = adc_red;
    adc_ir_h  = adc_ir;

    filter_index = 0;
    filter_count = 0;
    ppg_running  = false;
    ppg_sample_count = 0;

    if (ppgQueue == NULL)
    {
        ppgQueue = xQueueCreate(30, sizeof(uint16_t));
        configASSERT(ppgQueue != NULL);
    }
}

void PPG_Start(void)
{
    filter_index = 0;
    filter_count = 0;
    ppg_sample_count = 0;

    dbg_start_called = 1;
    ppg_running = true;

#ifdef USE_SIMULATION
    uart_ready = 0;
    HAL_UART_Receive_DMA(&huart2, usart_rx_buffer, BUFFER_SIZE); // <- questa riga
#endif

}

void PPG_Stop(void)
{
    ppg_running = false;

#ifndef USE_SIMULATION
    // TODO: Replace simulation input with ADC DMA acquisition

    // HAL_ADC_Stop(...)
    // HAL_TIM_Base_Stop_IT(...)
#endif
}


bool PPG_IsRunning(void)
{
    return ppg_running;
}

void PPG_WaitUntilDone(void)
{
    while (ppg_running)
    {
        osDelay(1);
    }
}

#ifdef USE_SIMULATION
void PPG_PushSimulatedSample(uint16_t sample)
{
    adc_raw = sample & 0x0FFF;
    uart_ready = 1;
}
#endif

void PPG_ProcessStep(void)
{
    if (!ppg_running)
        return;

    uint16_t sample;

    if (xQueueReceive(ppgQueue, &sample, portMAX_DELAY) != pdPASS)
        return;

    filtered_signal = (float)PPG_FilterMA(sample);
    ppg_red_filtered = (uint32_t)filtered_signal;

    ppg_sample_count++;

    if (ppg_sample_count >= (PPG_TOTAL_SAMPLES))
    {
        ppg_running = false;
        PPG_Stop();
    }
}


/* ------------------------------------------------------------------------- */
/* UART ISR hook (simulation only)                                           */
/* ------------------------------------------------------------------------- */

#ifdef USE_SIMULATION
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart_rx_cnt++;

        uint16_t sample =
            ((uint16_t)usart_rx_buffer[1] << 8) |
             usart_rx_buffer[0];

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(ppgQueue, &sample, &xHigherPriorityTaskWoken);

        HAL_UART_Receive_DMA(&huart2, usart_rx_buffer, BUFFER_SIZE);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
#endif
