/**
 ******************************************************************************
 * @file    ppg_processing.h
 * @author  A. Bellina
 * @brief   PPG processing module: filtering, HR & SpO2 computation,
 *          optional LED autocalibration (real hardware only).
 *
 * @details
 * This module provides a clean processing pipeline for PPG signals
 * (RED/IR channels). It supports:
 *   - Moving Average filtering
 *   - HR estimation
 *   - SpO2 estimation (ratio-of-ratios method)
 *   - Optional autocalibration to select LED PWM levels
 *   - Support for real hardware or simulation mode
 *
 * The acquisition is driven externally (e.g. TIM interrupt @ 100 Hz).
 *
 * Preprocessor flags:
 *   - USE_SIMULATION: disables ADC reads and enables UART-driven samples
 *   - USE_AUTOCALIBRATION: enables PWM autocalibration (real mode only)
 *
 * Selected variables are exposed for JLink/JScope plotting.
 ******************************************************************************
 */

#ifndef PPG_PROCESSING_H
#define PPG_PROCESSING_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------------------- */
/* Configuration                                                             */
/* ------------------------------------------------------------------------- */

/** UART buffer size for simulation (ADC sample = 2 bytes) */
#define BUFFER_SIZE            2U

/** Number of samples for one PPG acquisition session */
#define PPG_FS              100
#define PPG_WINDOW_SEC      5
#define PPG_NUM_WINDOWS     6

#define PPG_TOTAL_SAMPLES   (PPG_FS * PPG_WINDOW_SEC * PPG_NUM_WINDOWS)   /**< 30 s @ 100 Hz */

/** Moving Average filter window */
#define PPG_FILTER_WINDOW      16U

/** Number of samples for photodiode settling time */
#define SETTLING_TIME          15U     /**< 15 ms @ 100 Hz */

/* ------------------------------------------------------------------------- */
/* Public data (visible for JLink / JScope)                                   */
/* ------------------------------------------------------------------------- */

#ifdef USE_SIMULATION
/** Last simulated ADC input sample (12-bit) */
extern volatile uint16_t adc_raw;

/** UART RX buffer for simulated samples */
extern uint8_t usart_rx_buffer[BUFFER_SIZE];

/** Flag set when a new UART sample is ready */
extern volatile uint8_t uart_ready;
#endif

/** Last filtered signal (generic / simulation debug) */
extern volatile float filtered_signal;

/** Last filtered RED sample */
extern volatile uint32_t ppg_red_filtered;

/** Last filtered IR sample */
extern volatile uint32_t ppg_ir_filtered;

/** Last computed heart rate (bpm) */
extern volatile float ppg_heart_rate_bpm;

/** Last computed SpO2 (%) */
extern volatile float ppg_spo2_percent;

/* ------------------------------------------------------------------------- */
/* Public API                                                                */
/* ------------------------------------------------------------------------- */

/**
 * @brief Initialize the PPG processing module.
 *
 * @param[in] adc_red ADC handle for RED channel (ignored in simulation).
 * @param[in] adc_ir  ADC handle for IR channel (ignored in simulation).
 *
 * @note Must be called once at startup.
 */
void PPG_Init(ADC_HandleTypeDef *adc_red, ADC_HandleTypeDef *adc_ir);

/**
 * @brief Start a new PPG acquisition session.
 *
 * Resets internal state and enables sampling.
 * The acquisition will automatically stop after PPG_TOTAL_SAMPLES.
 */
void PPG_Start(void);

/**
 * @brief Stop PPG acquisition and processing
 *
 * Safely stops the PPG measurement, disables running flag
 * and allows the PPG task to exit or wait for next start.
 */
void PPG_Stop(void);


/**
 * @brief Execute one PPG processing step.
 *
 * This function must be called periodically (e.g. 100 Hz),
 * typically from a timer interrupt.
 */
void PPG_ProcessStep(void);

/**
 * @brief Check if PPG acquisition is currently running.
 *
 * @retval true  Acquisition active
 * @retval false Acquisition completed or not started
 */
bool PPG_IsRunning(void);

/**
 * @brief Block until PPG acquisition is complete.
 *
 * @note Intended for use inside a FreeRTOS task.
 */
void PPG_WaitUntilDone(void);

/**
 * @brief Push one simulated ADC sample (UART mode only).
 *
 * @param[in] sample 12-bit ADC value
 */
void PPG_PushSimulatedSample(uint16_t sample);

/**
 * @brief Run LED autocalibration (real hardware only).
 *
 * Determines PWM levels for RED/IR LEDs to maximize ADC dynamic range.
 *
 * @note Does nothing if USE_SIMULATION is defined.
 */
void PPG_RunAutocalibration(void);

#endif /* PPG_PROCESSING_H */
