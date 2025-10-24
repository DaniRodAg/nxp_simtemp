# nxp_simtemp

**Simulated Temperature Device with Real-Time Plotting**

---

## Overview
`nxp_simtemp` is a Linux kernel module and user-space GUI project that simulates a temperature sensor. It provides:

- A **Linux kernel module** `simtemp.ko` which can be loaded into the kernel using `sudo insmod <Origin folder>/simtemp.ko`
- A **Device Tree Blob Overlay** `simtemp.dtbo` which registers and describes a new device `simtemp`
- A **miscellaneous character device** `/dev/simtemp` to read simulated temperature values.
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
- Configurable sampling time.
- Configurable Threshold.
- Exposes readings via `/dev/simtemp`.
- Alerts when temperature exceeds a configurable threshold.
- Alerts when there are new temperature readings.

### User-Space Python Application
- Uses `select.epoll()` to monitor `/dev/simtemp`.
- Parses temperature, time, and alert flags from device output.
- Displays:
  - Real-time plot of temperature data.
  - Vertical buffer of last 8 readings in batches.

### User-Space C++ Application
- Uses `select.epoll()` to monitor `/dev/simtemp`.
- Prints drivers flags from device output.

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
There is a `run_demo.sh` file which can be ran inside the `/simtemp/scripts/` and it loads the module and runs the python app. 
It also changes the parameters of the driver. Those changes are exposed by the kernel and can be seen using the command `sudo dmesg -W` 
***Note:*** This bock the terminal input so a new one shall be used when running this command.

### Kernel Module
#### 1. Insert the Kernel Module
Inside the folder `/simtemp/kernel/` are located the loadable kernel module file app and the .odb file which are necesary when building the module. 
```bash
cd /nxp_simtemp/simtemp/kernel
sudo dtoverlay simtemp.dtbo
sudo insmod simtemp.ko
```
**dtoverlay** the command `sudo dtoverlay simtemp.dtbo` registers the device tree blob inside the device tree to make it accesible.
**insmod** the command `sudo insmod simtemp.ko` inserts the module in the kernel.

#### 2. Access the sensor readings
To access the sensor readings, these are exposed via `/dev/simtemp`.
And can be viewed by running the `sudo cat /dev/simtemp` which immediately displays the last 8 readings.

#### 3. Configure the sensor characteristics
The module exposes a `/sys/class/misc/simtemp/` miscellaneous device folder with configurable attributes `mode` `threshold_mC` `sampling_ms`


### User-Space C


### 3. User-Space GUI
Using a virtual environment (recommended)

cd user
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install pyqt5 pyqtgraph
python app.py

## Usage
### Device Interaction

Read temperature:

cat /dev/simtemp


Write mode:

echo "NOISY" > /dev/simtemp


Temperature threshold alerts are signaled via epoll.

---


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
