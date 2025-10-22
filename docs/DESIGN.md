```mermaid
flowchart TB
    %% ===========
    %% USER SPACE
    %% ===========
    subgraph USER_SPACE["User Space"]
        APP["User Application\n(read, write, ioctl, epoll)"]
    end

    %% ===========
    %% KERNEL SPACE
    %% ===========
    subgraph KERNEL_SPACE["Kernel Space"]
        SYSFS["SYSFS\nStores driver and device info"]
        ATTR["SYSFS Attributes\nExpose configuration controls"]
        VFS["VFS Layer\nDispatches syscalls to file_operations"]
        CHAR_FILE["Character Device File\n(/dev/mydevice)"]
        DRIVER["Character Device Driver\n(read, write, poll, ioctl)"]
        ISR["Interrupt Handler\nHandles hardware interrupts"]
        WAITQ["Wait Queue\nBlocks until data is ready"]
        WORKQ["Work Queue\nDefers heavy work to kernel thread"]
        HARDWARE["Hardware Device\n(Sensor, Peripheral, Controller)"]
    end

    %% ===========
    %% CONNECTIONS
    %% ===========

    %% Control Path
    SYSFS --> ATTR --> DRIVER

    %% Data Path
    APP -->|"syscalls"| VFS --> CHAR_FILE --> DRIVER
    DRIVER --> HARDWARE
    HARDWARE -->|"interrupt"| ISR --> WORKQ
    WORKQ --> DRIVER
    DRIVER --> WAITQ
    WAITQ -->|"wake up"| APP

    %% Epoll notification path
    DRIVER -->|"notify readiness"| EPOLL["Epoll Events"]
    EPOLL --> APP
