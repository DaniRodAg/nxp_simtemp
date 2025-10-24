```mermaid
flowchart TB
    %% ===========
    %% USER SPACE
    %% ===========
    subgraph USER_SPACE["User Space"]
        APP["User Application\n(open, read, write, epoll, close)"]
    end

    %% ===========
    %% KERNEL SPACE
    %% ===========
    subgraph KERNEL_SPACE["Kernel Space"]
        subgraph MISC_DEVICE_DRIVER["Misc Device Driver"]
            direction TB
            note1["**MISC_DEVICE:** Exposes a character device with a fixed **major number (10)**. Simplifies driver initialization by automatically creating the device node inside **/sys/class/misc/** and managing memory allocation."]
            
            CHAR_FILE["**Character Device File**  
            `/dev/simtemp` — exposed through misc device driver functions."]
            
            SYSFS["**SYSFS Interface**  
            Virtual filesystem for exposing kernel objects (kobjects)."]
            
            ATTR["**Attributes (ATTR)**  
            Virtual files that expose driver configuration.  
            Controlled by file operation functions (`read`, `write`, `epoll`, `ioctl`)."]

            EPOLL["**EPOLL** Event notification system. Uses kernel-level interrupts to wake a process when an event occurs on a registered file descriptor"]
            WAITQ["**Wait Queues** Mechanism used to manage threads or processes that are temporarily unable to proceed because they are waiting for a specific condition to become true or an event to occur."]
        end
        SPINLOCK["**SPINLOCK:** Locks memory space while working on it, so no other process can access at the same time."]
        WORKQ["Work Queue\nDefers heavy work to kernel thread. Queues specific work to avoid conflicts when we have a concurrent event."]
    end

    %% ===========
    %% CONNECTIONS
    %% ===========

    %% Control Path
    SYSFS --> ATTR

    %% Data Path
    APP -->|"EPOLL Instance"| EPOLL --> READ --> CHAR_FILE --> APP
    HARDWARE --> WORKQ --> SPINLOCK --> WRITE --> WAITQ 

    KERNEL_SPACE --> |"**Write()** Copies the data from the kernel space into the user space"| WRITE --> USER_SPACE
 
    %% Epoll notification path
    WAITQ -->|"wake up"| EPOLL --> APP

```
# Interactions

## User Space (APP)

- The application opens /dev/simtemp and registers the file descriptor using epoll.
- When data is available or the threshold event triggers, the kernel wakes the wait queue, which signals the epoll instance.
- The app then performs a non-blocking read() to fetch temperature data and updates the GUI (plot and buffer).

## Kernel Space (Driver)
- The driver is implemented as a miscellaneous device, simplifying initialization (fixed major number and automatic /dev/simtemp node creation).
- A high-resolution timer (hrtimer) periodically enqueues temperature sampling work into a workqueue.
- The workqueue handler generates a new temperature sample, writes it into a circular buffer, and signals the wait queue if a new event or threshold condition is met.
- The poll/epoll system call in user space is awakened through wake_up_interruptible(), triggering a notification event.

## Configuration Interface (SYSFS / ATTR)
- Driver parameters such as mode, threshold, and sampling period are exposed via /sys/class/misc/simtemp/.
- These virtual files are tied to device_attribute handlers that implement show() and store() functions for reading and writing configuration values.
- This allows user-space tools to change runtime parameters without recompiling or reloading the module.

##🔒 Locking Choices
- Spinlocks are used in the workqueue handler and circular buffer to protect data updates that may occur in interrupt or softirq context.
spin_lock_irqsave(&lock, flags);
/* update buffer, state */
spin_unlock_irqrestore(&lock, flags);


They’re chosen because:
Execution time inside the critical section is very short.
The context cannot sleep (e.g., timer callback).
Mutexes are not used here, since the operations that modify temperature samples occur inside the workqueue or interrupt context, where sleeping is not allowed.

## ⚙️ API Design Choices
Interface	Purpose	Why Used
sysfs attributes    |    Configure parameters (mode, sampling_ms, threshold_mC)	Human-readable interface for runtime tuning. Persistent via /sys/class/misc/simtemp/.
read()    |	Transfer temperature data to user space	Simple, synchronous mechanism. Works naturally with poll/epoll.
poll() / epoll()    |	Notify user-space on new data or threshold alert	Efficient event-driven mechanism; no busy-waiting.

## 🌳 Device Tree Mapping

The Device Tree Overlay (simtemp.dtbo) registers a node compatible with "nxp,simtemp".

When the kernel loads the overlay, the probe() function of the driver is called automatically for this compatible string.

Defaults:
If no device tree entry is provided, the driver falls back to built-in defaults (e.g., mode = NORMAL, sampling = 1000 ms, threshold = 30000 m°C).











