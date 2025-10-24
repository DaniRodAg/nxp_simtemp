# nxp_simtemp

**Simulated Temperature Device with Real-Time Plotting**

---

## Overview
`nxp_simtemp` is a Linux kernel module and user-space GUI project that simulates a temperature sensor. It provides:

- A **miscellaneous character device** `/dev/simtemp` to read simulated temperature values.
- **Temperature modes**: `RAMP`, `NOISY`, `NORMAL`, with configurable behavior.
- **Threshold alert mechanism** using epoll / poll.
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
- Exposes readings via `/dev/simtemp`.
- Alerts when temperature exceeds a configurable threshold.

### User-Space Python Application
- Uses `select.epoll()` to monitor `/dev/simtemp`.
- Parses temperature, time, and alert flags from device output.
- Displays:
  - Real-time plot of temperature vs. delta time.
  - Vertical buffer of last 8 readings in batches.
- Handles GUI safely with PyQt5 signals to avoid threading issues.

---

## Installation

### 1. Kernel Module
```bash
git clone https://github.com/DaniRodAg/nxp_simtemp.git
cd nxp_simtemp
make
sudo insmod simtemp.ko
ls /dev/simtemp

### 2. User-Space GUI

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
