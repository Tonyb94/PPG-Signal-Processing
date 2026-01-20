#   Real-Time PPG Signal Processing System (STM32 + FreeRTOS)
##  Architecture-focused embedded firmware for medical-grade devices

This repository contains the design and implementation of a **real-time embedded PPG signal processing system** targeting **medical-grade embedded applications**.

The project is developed for an **STM32F411RE MCU** and focuses on **deterministic timing, RTOS architecture, and hardware-driven signal acquisition**, rather than on pure algorithmic complexity.

The system is designed to be **modular, testable, and extensible**, following embedded best practices commonly adopted in medical and industrial devices.

---
#  ⚠️ **Note on project scope**  
> This project focuses on **embedded architecture, RTOS design, and timing correctness**.  
> Biomedical algorithms (HR, SpO₂) are intentionally left as *work in progress* and will be
> validated offline before full MCU integration.

---
## 🧠 Firmware Architecture Overview

The firmware is structured as a **real-time, RTOS-based pipeline** designed to process PPG samples with deterministic timing and clear separation of responsibilities.

A **hardware timer (TIM9)** defines the system sampling rate (**100 Hz**) and acts as the single timing reference for both simulation and real hardware operation.

At each timer tick:
1. A new sample is acquired (real ADC or simulated UART input)
2. The sample is forwarded to the processing task through an RTOS queue
3. Signal processing and system-level logic are executed in task context

This approach ensures **predictable timing**, **bounded latency**, and **clean task-level execution**.

---

## 🧩 RTOS Task Model

The firmware uses **FreeRTOS (CMSIS-RTOS v2)** and is organized into independent tasks, each with a well-defined responsibility:

| Task              | Status | Responsibility |
|-------------------|--------|----------------|
| **PPG Processing** | ⚠️ Partial | Real-time signal filtering and feature extraction |
| **Battery Monitor** | ✅ Implemented | Battery voltage monitoring and alarm generation |
| **Data Logger** | 🚧 Stub | SD card logging via SPI |
| **Display** | 🚧 Stub | User feedback and system status |
| **WiFi / MQTT** | 🚧 Stub | Remote telemetry and device communication |

Tasks communicate exclusively through **RTOS primitives** (queues, semaphores), avoiding shared-state coupling.

---

## ⏱ Timing and Synchronization Strategy

- **TIM9** defines the global sampling rate (100 Hz)
- Interrupt Service Routines are used only for:
  - sample framing
  - event notification
  - restarting non-blocking peripheral transfers (DMA / UART)
- All signal processing and system logic runs in **task context**
- Tasks block on RTOS objects (queues, semaphores, delays)
- No busy-waiting or polling is used

This model guarantees **deterministic execution**, efficient CPU usage, and scalability as system complexity grows.

---

## 🚦 Interrupt Design Philosophy

Interrupts are deliberately kept minimal to preserve system stability:

- No algorithmic processing inside ISRs
- No blocking RTOS calls
- No dynamic memory allocation
- Constant-time execution only

ISRs act as **event sources**, while all decision-making and computation is handled by RTOS tasks.

---

## 🧪 Simulation-Driven Development

To support early development and testing, the firmware includes a **simulation mode** that mirrors the real hardware execution flow:

- The MCU remains the timing master
- Synthetic PPG samples are generated in Python
- UART is used as a virtual ADC interface
- The same RTOS tasks and processing pipeline are executed in both modes

This allows validation of:
- task scheduling
- queue sizing
- real-time behavior
- system stability under load

without requiring physical sensors.

---

## 📊 Timing & Performance Summary

- Sampling frequency: **100 Hz (hardware-timed)**
- Processing: **1 sample per RTOS cycle**
- Queue depth sized to absorb transient jitter
- No missed samples observed during simulation

The architecture is ready for future integration of:
- DMA-based ADC acquisition
- real optical PPG sensors

---

## Development Tools

### Hardware
- **STM32F411RE** (Nucleo board)
- PPG sensor *(planned)*
- SD Card module via SPI *(planned)*

### Software
- STM32CubeMX
- FreeRTOS (CMSIS-RTOS v2)
- VS Code + Cortex-Debug + ARM Toolchain
- SEGGER J-Link + J-Scope
- Python 3.9 (signal simulation and offline analysis)
- draw.io / diagrams.net

---

## Getting Started

## MCU Pin Configuration (Summary)

| Peripheral | Pins | Notes |
|----------|------|------|
| UART (Simulation) | PA3 | Virtual ADC interface |
| Timer | TIM9 | 100 Hz sampling |
| R PWM| PA6| Red light modulation|
|IR PWM| PA5| Infrared light modulation|
|Battery alarm| PA7 | External led GPIO |

### Build the STM32 Project

```bash
cd HR_SPO2_computing_dev

# Simulation build
cmake --build --preset SimulatedDebug

# Hardware build
cmake --build --preset Debug
```

## VS Code Workflow

1. Open the project folder in VS Code
2. Open the Command Palette
3. Select **CMake: Select Configure Preset**
4. Choose `SimulatedDebug` or `Debug`
5. Run **CMake: Configure**
6. Build the project

Use the provided `launch.json` file for debugging.  
Do not overwrite the existing configuration.

---

## Debugging

Supported debugging options:

- SEGGER J-Link (recommended)
- ST-Link
- STM32CubeIDE
- VS Code with Cortex-Debug extension

Real-time signal inspection is performed using **SEGGER J-Scope**.

---

## Python Simulation Environment

The Python environment is used to generate synthetic PPG samples and simulate the sensor during development.

Setup steps:

```bash
python -m venv SPO2_project_venv
cd SPO2_project_venv\Scripts
activate
pip install -r requirements.txt
```

## 📚 API Documentation (Doxygen)

The embedded firmware is documented using **Doxygen** to improve readability,
maintainability, and onboarding for new developers.

The documentation focuses on:
- Module responsibilities
- Public APIs
- Data flow between processing blocks
- Timing-related design choices

### 🔗 Accessing the documentation

To view the generated documentation locally:

1. Clone the repository
2. Open the following file doc\Doxy\index.html 


> Note: The documentation emphasizes **architecture and interfaces** rather than
final biomedical algorithms, which are still under development.


## 🧩 System Architecture

The firmware architecture is based on a **modular, RTOS-driven design**, suitable
for medical and industrial embedded systems.

Key design principles:
- Clear separation between acquisition, processing, and application logic
- Deterministic timing using hardware timers
- Decoupling of signal acquisition and processing
- Support for both **real hardware acquisition** and **PC-based simulation**

The system is described using **multi-level DeMarco functional diagrams**.

---

---

## Diagrams

### Level 0: System Context
![Level 0](diagrams/level0_system_context.svg)

**Overview:**  
Shows the interaction between the device and external entities such as:
- PPG sensor
- User interface
- Power supply
- Data logging subsystem

---

### Level 1: Main Processes
![Level 1](diagrams/level1_main_processes.svg)

**Overview:**  
Illustrates the main functional blocks:
1. HR / SpO₂ Computation
2. Battery Monitoring & Alarm
3. Data Logging

---

### Level 2: HR/SpO2 Computation Module
![Level 2 HR/SpO2](diagrams/level2_hr_spo2.svg)

**Components:**  
Details the signal processing pipeline:
- Raw PPG acquisition
- Digital filtering
- Feature extraction
- Output data flow

---

### Level 2: Battery Alarm Module
![Level 2 Battery Alarm](diagrams/level2_battery_alarm.svg)

**Components:**  
Describes battery monitoring and threshold-based alert generation.

---

### Level 2: Data Logger Module
![Level 2 Data Logger](diagrams/level2_data_logger.svg)

**Components:**  
Describes SD card logging through SPI, with data stored in a lightweight,
structured format suitable for future interoperability.


## Project Structure of STM32 MCU

```
HR_SPO2_computing_dev/
├───.vscode
├───cmake/
│   └───stm32cubemx
├───Core/
│   ├───Inc
│   │       battery_monitor.h
│   │       FreeRTOSConfig.h
│   │       main.h
│   │       ppg_processing.h
│   │       stm32f4xx_hal_conf.h
│   │       stm32f4xx_it.h
│   └───Src
│           battery_monitor.c
│           freertos.c
│           main.c
│           ppg_processing.c
│           stm32f4xx_hal_msp.c
│           stm32f4xx_hal_timebase_tim.c
│           stm32f4xx_it.c
│           syscalls.c
│           sysmem.c
│           system_stm32f4xx.c
├───Drivers/
│   ├───CMSIS/
│   │   ├───Device/ST/STM32F4xx
│   │   │   ├───Include
│   │   │   └───Source/Templates
│   │   └───Include
│   └───STM32F4xx_HAL_Driver/
│       ├───Inc
│       └───Src
├───FATFS/
│   ├───App
│   └───Target
└───Middlewares/
    └───Third_Party
        ├───FatFs/src/option
        └───FreeRTOS/Source
            ├───CMSIS_RTOS_V2
            ├───include
            └───portable
                ├───GCC/ARM_CM4F
                └───MemMang

```

## 📌 Project Scope & Status

This project is intentionally structured as a **work-in-progress embedded system**.

Current focus:
- RTOS architecture
- Deterministic timing
- Simulation framework
- Modular firmware design

Out of scope (for now):
- Clinical validation
- Final HR / SpO₂ algorithms

---

## 📄 License

This project is provided for educational and portfolio purposes.

