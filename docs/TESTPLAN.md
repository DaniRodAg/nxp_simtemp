# nxp_simtemp Test Plan

## 1. Introduction
**Project Name:** nxp_simtemp  
**Purpose:** Validate the functionality, reliability, and performance of the simulated temperature device and its user-space interfaces (Python GUI, misc device).  
**Scope:** Testing includes kernel module, `/dev/simtemp` character device, sysfs attributes, epoll notifications, and Python GUI plotting.  
**Tested Environment:** Linux (Raspberry Pi OS / Debian), Python 3.11+, PyQt5, PyQtGraph.

---

## 2. Test Objectives
- Verify kernel module loads and unloads correctly.
- Confirm `/dev/simtemp` exposes temperature readings.
- Validate temperature modes (`RAMP`, `NOISY`, `NORMAL`).
- Check threshold alerts and epoll notifications.
- Verify sampling time configuration.
- Test GUI for correct plotting, delta-time calculation, and buffer display.
- Validate error handling for invalid inputs or extreme conditions.

---

## 3. Test Items
- `simtemp.ko` kernel module
- `/dev/simtemp` character device
- Sysfs attributes: `mode`, `threshold_mC`, `sampling_ms`
- Python GUI for real-time plotting
- Optional user-space C test applications
- Epoll notification mechanisms

---

## 4. Features to be Tested

| Feature | Test Description | Expected Result |
|---------|----------------|----------------|
| Module load/unload | `insmod simtemp.ko` and `rmmod simtemp` | Module loads/unloads without errors; `/dev/simtemp` node appears/disappears |
| Temperature reading | `cat /dev/simtemp` | Shows last 8 readings in correct format, updated periodically |
| Mode `RAMP` | Write `RAMP` to `/sys/class/misc/simtemp/mode` | Temperature increases steadily each sample |
| Mode `NOISY` | Write `NOISY` | Temperature changes randomly within expected range |
| Mode `NORMAL` | Write `NORMAL` | Temperature fixed at configured value (e.g., 25°C) |
| Threshold alert | Set `threshold_mC`, generate temp > threshold | Epoll wakes user-space; GUI shows alert |
| Sampling time | Set `sampling_ms` | Temperature readings update at correct interval |
| GUI plotting | Run Python app | Temperature plot updates in real-time; delta time is correct; buffer displays last 8 readings |
| Error handling | Invalid mode or malformed write | Driver resets to `NORMAL` without crashing |
| High frequency | Test 10 kHz sampling | Module handles correctly or documents performance limits |

---

## 5. Test Approach
- **Manual Tests:** Load kernel module, change modes, check `/dev/simtemp`, run Python GUI.  
- **Automated Tests:** Python scripts reading `/dev/simtemp`, verifying temperature values and threshold alerts.  
- **Stress Tests:** Simulate high-frequency sampling, multiple GUI instances.  
- **Boundary Tests:** Max/min temperature values, buffer overflow, epoll notifications firing.

---

## 6. Pass/Fail Criteria
- **PASS:**  
  - Kernel module loads/unloads cleanly.  
  - Temperature readings are accurate.  
  - Threshold alerts and epoll notifications work.  
  - Python GUI displays data correctly.  
- **FAIL:**  
  - Kernel panic or crash.  
  - Missing or incorrect readings.  
  - Alerts not triggered.  
  - GUI fails to update or crashes.

---

## 7. Test Deliverables
- Test plan document.  
- Test result logs.  
- Screenshots of GUI and `/dev/simtemp` outputs.  
- Automated test scripts (optional).

---

## 8. Risks / Considerations
- High-frequency sampling may saturate CPU or kernel timer.  
- Concurrent reads from multiple processes may require careful locking.  
- Python GUI should handle buffer overflows and missed updates gracefully.  
- Sysfs interface only exposes attributes; complex control might require ioctl if extended functionality is added.

---

## 9. References
- Kernel documentation: [miscdevice](https://www.kernel.org/doc/html/latest/driver-api/misc-devices.html)  
- Python GUI libraries: [PyQt5](https://www.riverbankcomputing.com/software/pyqt/intro), [PyQtGraph](http://www.pyqtgraph.org/)  
- Epoll: [man epoll](https://man7.org/linux/man-pages/man7/epoll.7.html)  
- Device Tree documentation: [https://www.kernel.org/doc/Documentation/devicetree/](https://www.kernel.org/doc/Documentation/devicetree/)
