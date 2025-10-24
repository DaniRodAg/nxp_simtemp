# nxp_simtemp

**Simulated Temperature Device with Real-Time Plotting**

---

## Overview
`nxp_simtemp` is a Linux kernel module and user-space GUI project that simulates a temperature sensor. It provides:

- A **Linux kernel module** `simtemp.ko` that can be loaded into the kernel using `sudo insmod <path_to>/simtemp.ko`
- A **Device Tree Blob Overlay** `simtemp.dtbo` that registers and describes a new device `simtemp`
- A **miscellaneous character device** `/dev/simtemp` for reading simulated temperature values.
- **Temperature modes**: `RAMP`, `NOISY`, `NORMAL`, with configurable behavior.
- Configurable **Sampling time**.
- **Threshold alert mechanism** using epoll / poll, with configurable behavior.
- Real-time plotting of temperature readings with PyQt5 and PyQtGraph.
- A **Python GUI** that displays the last N readings, delta times, and allows monitoring of temperature batches.

---

## Features

### Kernel Module
- Periodic high-resolution timer (`hrtimer`) generates simulated temperature readings.
- Configurable modes:
  - `RAMP`: temperature increases steadily.
  - `NOISY`: random temperature readings.
  - `NORMAL`: fixed temperature.
- Adjustable sampling period and temperature threshold.
- Exposes data through /dev/simtemp.
- Raises alerts when:
  - New temperature data is available.
  - The temperature exceeds the threshold.

### User-Space Python Application
- Uses `select.epoll()` to monitor `/dev/simtemp`.
- Parses temperature, time, and alert flags from device output.
- Displays:
  - Real-time plot of temperature data.
  - Vertical buffer of last 8 readings in batches.

### User-Space C++ Application
- Uses `select.epoll()` to monitor `/dev/simtemp`.
- Displays driver flags and readings in the terminal.
---

## Installation

### 1. Clone the repo
`git clone https://github.com/DaniRodAg/nxp_simtemp.git`

### 2. Configure the enviroment
For the testing of the device driver a **Raspberry5** was used.
```bash
sudo raspi-config
sudo yum install system-config-keyboard
```
### 3. Install dependencies
```bash
sudo apt-get install python3-pyqt5
sudo apt-get install python3-pyqt5 QLabel
```

## Usage
A helper script run_demo.sh is included in /simtemp/scripts/.
This script:
- Loads the kernel module.
- Runs the Python GUI.
- Configures the driver parameters dynamically.
You can also view kernel logs live using: `sudo dmesg -W` 

⚠️ Note: This blocks the terminal input — open a new terminal if needed.

### Kernel Module
#### 1. Load the Module
Navigate to the kernel folder:
```bash
cd /nxp_simtemp/simtemp/kernel
sudo dtoverlay simtemp.dtbo
sudo insmod simtemp.ko
```
**dtoverlay** the command `sudo dtoverlay simtemp.dtbo` registers the device tree blob inside the device tree to make it accesible.
**insmod** the command `sudo insmod simtemp.ko` inserts the module in the kernel.

#### 2. Read Sensor Data
Temperature readings are available via: `sudo cat /dev/simtemp`.
This shows the last 8 temperature readings.

#### 3. Configure Parameters
The module exposes a sysfs interface:
`/sys/class/misc/simtemp/`
You can modify:
- mode
- threshold_mC
- sampling_ms

**Example:**
`echo "NOISY" | sudo tee /sys/class/misc/simtemp/mode`
`echo "20000" | sudo tee /sys/class/misc/simtemp/threshold_mC`

#### User-Space C
Navigate to the kernel folder:
```bash
cd /nxp_simtemp/simtemp/kernel
```
Run the application 
```bash
sudo ../kernel/main_app
```

⚠️ Note: The application only runs after the module is loaded into the kernel.

#### User-Space GUI
Navigate to the user folder:
```bash
cd /nxp_simtemp/simtemp/user/gui
```
Run the application 
```bash
sudo python3 app.py
```
⚠️ Note: The application only runs after the module is loaded into the kernel.

## GUI

Launches a window with:

Temperature plot in real-time.

Vertical buffer of last 8 readings.

Updates automatically as new data arrives.

---

## Implementation Notes

Kernel module uses hrtimer for periodic updates.

miscdevice interface allows safe user-space access.

wait_queue + poll + epoll mechanism signals user-space apps.

Python GUI uses PyQt5 + PyQtGraph:

DeviceReader runs in a separate thread to read /dev/simtemp.

Emits signals for thread-safe GUI updates.

Buffers readings and computes delta times.

---

## License

MIT License

---

## Author 
Daniel Josue Rodriguez Agraz
GitHub: DaniRodAg
