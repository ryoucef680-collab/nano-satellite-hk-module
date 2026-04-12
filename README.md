# nano-satellite-hk-module
# 🛰️ Nanosatellite Housekeeping Module — HK/WOD Prototype

> **Internship project — Satellite Development Centre (CDS/ASAL)**  
> Study and familiarisation with nanosatellite flight software,  
> exploration of the software architecture and prototyping of a Housekeeping module.

---

## 📄 Internship Report

The full technical report documenting the architecture analysis, function documentation,
FreeRTOS orchestration design, and simulation results is available here:

📎 **[Read the Internship Report (PDF)](./Docs/Report_HK_nanosat.pdf)**

The report covers:
- GlobusSat open-source reference architecture
- Housekeeping (HK) and Whole Orbit Data (WOD) definitions
- EM system architecture and software layers
- Complete function documentation (`hk.c` — 8 functions)
- FreeRTOS task orchestration (`HousekeepingTask`)
- Arduino Mega 2560 simulation results

---

## 🧭 Project Overview

This repository contains the prototype implementation of a **Housekeeping (HK) module**
for a nanosatellite On-Board Computer (OBC) based on the ISIS iOBC (ARM9 + FreeRTOS).

Since the iOBC hardware was not available during the internship, the module was
validated through two simulation layers:

| Layer | Platform | Purpose |
|-------|----------|---------|
| **Arduino simulation** | Arduino Mega 2560 + FreeRTOS | Validate task scheduling, queue communication, and telemetry periodicity |
| **Python logger** | Python 3 + pyserial | Read Serial Monitor output, parse WOD packets, save to CSV |
| **C++ iOBC stub** | C++17 (desktop) | Simulate the full iOBC-side logic without hardware |

---

## 📁 Repository Structure

```
nano-satellite-hk-module/
│
├── arduino/
│   └── hk_simulation/
│       └── hk_simulation.ino       # Main Arduino FreeRTOS sketch
│
├── cpp/
│   ├── include/
│   │   ├── WOD_Telemetry.h         # WOD_Telemetry_t struct definition
│   │   ├── TelemetryCollector.h    # Function declarations
│   │   └── stubs.h                 # Hardware stub declarations
│   ├── src/
│   │   ├── TelemetryCollector.cpp  # TelemetryCollectorLogic, Save functions
│   │   ├── GetCurrentWOD.cpp       # GetCurrentWODTelemetry implementation
│   │   └── stubs.cpp               # I2C / FRAM / filesystem stubs
│   └── main.cpp                    # Entry point — runs the HK loop
│
├── python/
│   ├── serial_logger.py            # Reads Serial Monitor, parses WOD lines
│   ├── plot_telemetry.py           # Plots vbat, currents, power over time
│   └── WOD_telemetry_2026~04~01_11;16;46.csv              # Example CSV output
│
├── docs/
│   └── nanosat.pdf                 # Full internship report
│
└── README.md
```

---

## ⚙️ How the Module Works

```
Power ON / OBC boot
        │
        ▼
    setup()  ─── creates queue, registers tasks
        │
        ▼
vTaskStartScheduler()
     /        \
    ▼            ▼
OBC_Task       HK_Task
(priority 2)   (priority 1)
generates WOD  receives WOD
sends to queue calls TelemetryCollectorLogic()
                        │
                        ▼
        TelemetryCollectorLogic()
          ├── EPS      every  4 s  →  TelemetrySaveEPS()
          ├── TRXVU    every  6 s  →  TelemetrySaveTRXVU()
          ├── ANT      every  8 s  →  TelemetrySaveANT()
          ├── SOLAR    every 10 s  →  TelemetrySaveSolarPanels()
          └── WOD      every  5 s  →  TelemetrySaveWOD()
                                              │
                                              ▼
                                     FRAM file system
                                  c_fileWrite() — persistent storage
```

---

## 🔧 Arduino Simulation — Step by Step

### Requirements

| Tool | Version |
|------|---------|
| Arduino IDE | 2.x or later |
| Board | Arduino Mega 2560 |
| Library | `FreeRTOS by Richard Barry` (install via Library Manager) |

### Installation

**1. Clone the repository:**
```bash
git clone https://github.com/youcef-rahali/nano-satellite-hk-module.git
cd nano-satellite-hk-module
```

**2. Open the sketch in Arduino IDE:**
```
arduino/hk_simulation/hk_simulation.ino
```

**3. Install the FreeRTOS library:**
- Open Arduino IDE → **Tools → Manage Libraries**
- Search for `FreeRTOS`
- Install **"FreeRTOS by Richard Barry"**

**4. Select the correct board:**
- **Tools → Board → Arduino AVR Boards → Arduino Mega or Mega 2560**
- **Tools → Processor → ATmega2560**

**5. Select the correct port:**
- **Tools → Port → COMx** (Windows) or **/dev/ttyUSBx** (Linux/Mac)

**6. Upload and open Serial Monitor:**
- Click **Upload** (→)
- Open **Tools → Serial Monitor**
- Set baud rate to **9600**

### Expected Serial Monitor Output

```
========== OBC Task: generating telemetry ==========
  [OBC] Subsystem values generated.
---------- HK Task: collecting & saving ----------
[EPS] Saving EPS telemetry...
  raw_mb=2983  eng_mb=3241  raw_cdb=2912  eng_cdb=3561
  [FILE WRITE] -> EPS_RAW_MB_TLM
  [FILE WRITE] -> EPS_ENG_MB_TLM
[WOD] Saving WOD telemetry...
  vbat=3874 mV
  volt_3V3=3301 mV
  volt_5V=5023 mV
  current_3V3=134 mA
  current_5V=217 mA
  charging_power=512 mW
  consumed_power=298 mW
  free_memory=304512 bytes
  resets=3
  [FILE WRITE] -> WOD_TLM
  [HK] WOD Telemetry saved:
  Vbat:           3874 mV
  Sat Time:       12 sec
--------------------------------------------------
WOD,12,3874,3301,5023,134,217,512,298,304512,3
```

> The last line starting with `WOD,` is the structured CSV line consumed by the Python logger.

---

## 🐍 Python Logger — Step by Step

The Python logger connects to the Arduino over Serial, filters the `WOD,` lines,
and saves them to a CSV file for analysis and plotting.

### Requirements

```bash
pip install pyserial matplotlib pandas
```

### Run the logger

```bash
cd python
python serial_logger.py
```

Edit the top of `serial_logger.py` to match your serial port:

```python
SERIAL_PORT = "COM3"       # Windows : COM3, COM4 ...
                           # Linux   : /dev/ttyUSB0
                           # Mac     : /dev/cu.usbmodem...
BAUD_RATE   = 9600
OUTPUT_FILE = "wod_output.csv"
```

### CSV output format

```
sat_time,vbat,volt_3V3,volt_5V,current_3V3,current_5V,charging_power,consumed_power,free_memory,resets
12,3874,3301,5023,134,217,512,298,304512,3
15,3901,3298,5011,141,203,498,312,304100,3
```

### Plot the telemetry

```bash
python plot_telemetry.py
```

Generates plots of `vbat`, `current_3V3`, `charging_power`, and `consumed_power`
over time from the saved CSV file.

---

## 💻 C++ Desktop Simulation — Step by Step

Runs the full `TelemetryCollectorLogic()` loop on any desktop machine.
All I²C, FRAM, and filesystem calls are replaced by stubs returning
randomised values within realistic ranges.

### Requirements

- GCC / G++ with C++17 support, **or** CMake 3.15+

### Build and run (GCC)

```bash
cd cpp
g++ -std=c++17 -o hk_sim main.cpp src/*.cpp -I include
./hk_sim
```

### Build and run (CMake)

```bash
cd cpp
mkdir build && cd build
cmake ..
make
./hk_sim
```

### Expected output

```
[TelemetryCollectorLogic] t=0s
[EPS]   period=4s   -> TelemetrySaveEPS()         SAVED
[WOD]   period=5s   -> TelemetrySaveWOD()         SAVED
[TRXVU] period=6s   -> TelemetrySaveTRXVU()       SAVED
[ANT]   period=8s   -> TelemetrySaveANT()         SAVED
[SOLAR] period=10s  -> TelemetrySaveSolarPanels()  SAVED
```

---

## 📡 WOD Telemetry Fields

| Field | Description | Unit | Simulated range |
|-------|-------------|------|-----------------|
| `vbat` | Battery voltage | mV | 3600 – 4200 |
| `volt_3V3` | 3.3 V bus voltage | mV | 3250 – 3350 |
| `volt_5V` | 5 V bus voltage | mV | 4900 – 5100 |
| `current_3V3` | 3.3 V bus current | mA | 50 – 200 |
| `current_5V` | 5 V bus current | mA | 50 – 300 |
| `charging_power` | Solar panel output | mW | 200 – 800 |
| `consumed_power` | Power consumed | mW | 100 – 500 |
| `free_memory` | Filesystem free space | bytes | 100 000 – 512 000 |
| `corrupt_bytes` | Corrupted bytes | bytes | 0 – 10 |
| `number_of_resets` | OBC reset counter | — | 3 (fixed stub) |
| `sat_time` | Mission time since boot | sec | seconds since boot |

---

## 🔗 References

| # | Resource |
|---|----------|
| [1] | [GlobusSat v1 — TelemetryCollector.c](https://github.com/Rogozin-Tevel/GlobusSatProject/blob/master/GlobusSatProject/src/SubSystemModules/Housekeeping/TelemetryCollector.c) |
| [2] | [GlobusSat v2 — HouseKeeping.c](https://github.com/Rogozin-Tevel/GlobusSatProject-2.0/blob/d9581d09df3a7ad829da76a976a0bb0d0dbcc9b9/GlobusSatProject/Hoopoe3/src/sub-systemCode/Main/HouseKeeping.c) |
| [3] | [ISIS iOBC On-Board Computer](https://www.isispace.nl/product/on-board-computer) |
| [4] | [FreeRTOS Official Documentation](https://www.freertos.org/Documentation) |
| [5] | [ECSS-E-ST-70-41C Telemetry Standard](https://ecss.nl/standards/active-standards/) |

---

## 👨‍💻 Developed in

**Host organisation:** Satellite Development Centre — CDS/ASAL  
**Institution:** Higher School of Applied Sciences — ESSA Tlemcen  

**Date:** March 2026

---

## 📜 Licence

This project was developed as part of an academic internship at CDS/ASAL.  
All rights reserved © 2026 CDS/ASAL.
