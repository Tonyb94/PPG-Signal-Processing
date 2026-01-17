#ifndef APP_TASKS_H
#define APP_TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

/**
 * @defgroup AppTasks Application Tasks
 * @brief FreeRTOS application-level tasks
 * @{
 */

/**
 * @brief Task that computes Heart Rate and SpO2 from PPG signals
 * @param argument FreeRTOS task argument (unused)
 */
void Start_HR_SPO2_task(void *argument);

/**
 * @brief Task that monitors battery voltage and status
 * @param argument FreeRTOS task argument (unused)
 */
void Start_Battery_monitor(void *argument);

/**
 * @brief Task that publishes data over MQTT
 * @param argument FreeRTOS task argument (unused)
 */
void Start_publisher(void *argument);

/**
 * @brief Task that handles data logging to storage
 * @param argument FreeRTOS task argument (unused)
 */
void Start_Datalogging(void *argument);

/**
 * @brief Task that manages data visualization and display updates
 * @param argument FreeRTOS task argument (unused)
 */
void Start_Displaying(void *argument);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* APP_TASKS_H */
