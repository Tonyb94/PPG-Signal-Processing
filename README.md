# PPG-Signal-Processing Device
Signal Processing designed for STM32f411re MCU for a complete medical device project 

# PPG Signal Processing Device

This repository contains the design and implementation of a **PPG Signal Processing Device** with heart rate (HR) and SpO2 computation, battery alarm, and data logging functionality. The project is structured into modular components, represented in a multi-level DeMarco functional diagram.

A modular implementation of a **PPG-based signal processing system** running on an STM32F411RE MCU, featuring:

- Digital filtering  
- Heart Rate (HR) computation  
- SpO₂ computation  
- Battery level monitoring & alarm  
- Data logging to SD card  
- Extensible architecture for WiFi/MQTT/WebApp integration  

The architecture follows a **multi-level DeMarco functional diagram** for clarity and future scalability.

---

## 🛠 Development Tools

This project is developed using the following hardware and software stack:

### **Hardware**
- **STM32F411RE** (Nucleo board) — main MCU for signal processing  
- **PPG Sensor** (MAX30102 or compatible, planned)  
- **SEGGER J-Link** — debugging & J-Scope signal visualization  
- **SD Card Module via SPI** — data logging  

### **Software**
- **STM32CubeMX** — peripheral initialization  
- **VS Code + Cortex-Debug + ARM Toolchain** — development environment  
- **Python 3.9.13** — synthetic data generation and offline analysis  
- **SEGGER J-Scope** — real-time visualization of filtered signal  
- **draw.io / diagrams.net** — system diagrams  

---

## 📈 Real-Time Filtering Preview (STM32 + J-Scope)

Example of filtered PPG waveform captured during firmware testing:

![PPG Filtered Signal](images/jscope_filtered_signal.png)

---
## 🏗 Getting Started

### 1. STM32 Project Build

```bash
# Navigate to project folder
cd HR_SPO2_computing_dev

# Clean previous builds (optional)
rm -rf build/Debug build/SimulatedDebug

# Build project
# - If using CubeIDE: Open project -> Build
# - If using Makefile/CMake: make all
```
## Diagrams

### Level 0: System Context
![Level 0](diagrams/level0_system_context.svg)

**Overview:**  
Shows the overall system, including the PPG sensor, battery, user interface, and the main processing module. This level highlights the interaction between the device and external entities.

---

### Level 1: Main Processes
![Level 1](diagrams/level1_main_processes.svg)

**Overview:**  
Illustrates the three main modules of the system:
1. **HR/SpO2 Computation** – processes raw PPG signals to compute heart rate and oxygen saturation.  
2. **Battery Alarm** – monitors battery level and triggers alerts when low.  
3. **Data Logger** – stores processed HR/SpO2 data for later use.

---

### Level 2: HR/SpO2 Computation Module
![Level 2 HR/SpO2](diagrams/level2_hr_spo2.svg)

**Components:**  
- **PPG Signal Input** – receives raw sensor data.  
- **Digital Filtering** – applies moving average or other filters to clean the signal.  
- **Feature Extraction** – calculates heart rate and oxygen saturation values.  
- **Output Data Flow** – sends computed HR/SpO2 values to the Data Logger module.

---

### Level 2: Battery Alarm Module
![Level 2 Battery Alarm](diagrams/level2_battery_alarm.svg)

**Components:**  
- **Battery Monitor** – reads battery voltage level.  
- **Threshold Detection** – compares current voltage with predefined thresholds.  
- **Alert Generation** – triggers notifications to the user or device interface when battery is low.

---

### Level 2: Data Logger Module
![Level 2 Data Logger](diagrams/level2_data_logger.svg)

**Components:**  
- **Data Logger Manager** – receives HR/SpO2 data and formats it for storage.  
- **SPI Manager** – manages SPI communication to write data to SD card.  
- **SD Card Storage** – stores data in **JSON format**, lightweight and suitable for future integration with HL7 standards or web applications.

**Data Flow:**  

## Project Structure

```
PPG-Device/
├── diagrams/
│   ├── level0_system_context.svg
│   ├── level1_main_processes.svg
│   ├── level2_hr_spo2.svg
│   ├── level2_battery_alarm.svg
│   └── level2_data_logger.svg
├── src/
│   ├── hr_spo2_computation/
│   ├── battery_alarm/
│   └── data_logger/
└── README.md
```
