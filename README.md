# PPG Signal Processing System (STM32 + FreeRTOS)

This repository contains the design and implementation of a **real-time embedded PPG signal processing system** targeting **medical-grade embedded applications**.

The project is developed for an **STM32F411RE MCU** and focuses on **deterministic timing, RTOS architecture, and hardware-driven signal acquisition**, rather than on pure algorithmic complexity.

The system is designed to be **modular, testable, and extensible**, following embedded best practices commonly adopted in medical and industrial devices.

---

## Project Overview

The firmware implements a complete signal-processing pipeline for a PPG-based device, including:

- Real-time PPG signal acquisition 
- Digital filtering
- Heart Rate (HR) computation *(planned)*
- SpO₂ computation *(planned)*
- Battery monitoring and alarm task
- Data logging architecture (SD card, SPI) *(planned)*
- Simulation mode for development without physical sensors

The architecture is documented using **multi-level DeMarco functional diagrams**.

---

## Design Goals

The main design goals of this project are:

- Deterministic sampling using **hardware timers**
- Clear separation between **ISR, RTOS tasks, and processing logic**
- Minimal and safe ISR design
- RTOS-based synchronization using **queues and semaphores**
- Identical processing flow in **simulation and real hardware modes**
- Long-term maintainability and extensibility

These goals reflect typical constraints of **medical and safety-oriented embedded systems**.

---

## RTOS Architecture

The firmware uses **FreeRTOS (CMSIS-RTOS v2)** and is organized into independent tasks:

| Task            |status | Responsibility                                 |
|-----------------|-------|------------------------------------------------|
| PPG Processing  |Partially Implemented| Real-time filtering and signal processing  |
| Battery Monitor |Implemented| Battery voltage monitoring and alarm       |
| Data Logger | Stub | SD card logging via SPI |
| Display | Stub | User feedback and status visualization |
| WiFi / MQTT | Stub | Remote telemetry and device communication |

### Synchronization Model

- A **hardware timer (TIM9)** defines the system sampling rate (100 Hz)
- ISRs are limited to signaling mechanisms only
- Data exchange between ISR and tasks uses **FreeRTOS queues**
- No busy-waiting or polling inside tasks

This ensures **bounded latency and predictable execution timing**.

---

## Interrupt Design Philosophy

All interrupts are designed according to the following rules:

- No signal processing inside ISRs
- No blocking RTOS calls inside ISRs
- No dynamic memory allocation
- ISRs only:
  - push data to queues
  - release semaphores
  - notify tasks

This approach is aligned with best practices for **real-time and medical embedded systems**.

---

## Simulation-Driven Development

To decouple firmware development from sensor availability, the system supports a **simulation mode**:

- The MCU remains the **timing master**
- Synthetic PPG samples are generated externally using Python
- UART is used as a virtual ADC interface
- The same processing task is used in simulation and real modes

This allowed early validation of:
- task scheduling
- queue sizing
- real-time behavior
- system stability under load

---

## Timing & Performance

- Sampling frequency: **100 Hz (hardware-timed)**
- Processing rate: 1 sample per tick
- Queue depth sized to absorb jitter
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

**Data Flow:**  

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

---

## 🚧 Project Status

Current focus:
- Firmware architecture
- Timing control
- Simulation framework
- Modular processing pipeline

Planned next steps:
- Heart Rate computation
- SpO₂ computation
- Improved digital filtering
- Sensor integration
- Data logging refinement

---

## 📌 Notes for Recruiters

This project is intentionally structured as a **work-in-progress system**.
The focus is on:
- Embedded architecture
- RTOS usage
- Timing correctness
- Testability and scalability

Biomedical algorithms will be refined in later stages.

---

## 📄 License

This project is provided for educational and portfolio purposes.

