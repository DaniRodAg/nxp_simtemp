```mermaid
flowchart TB
    %% ===========
    %% USER SPACE
    %% ===========
    subgraph USER_SPACE[User Space]
        APP[User Application<br><br>• read(), write(), ioctl()<br>• uses epoll() for notifications]
    end

    %% ===========
    %% KERNEL SPACE
    %% ===========
    subgraph KERNEL_SPACE[Kernel Space]
        SYSFS[SYSFS<br>Stores driver & device info]
        ATTR[SYSFS Attributes<br>Expose configuration controls]
        VFS[VFS Layer<br>Dispatches syscalls to driver file_operations]
        CHAR_FILE[Character Device File<br>/dev/mydevice<br>Virtual interface for driver]
        DRIVER[Character Device Driver<br>Implements read(), write(), poll(), ioctl()]
        ISR[Interrupt Handler<br>Handles hardware interrupts]
        WAITQ[Wait Queue<br>Blocks processes until data ready]
        WORKQ[Work Queue<br>Defers heavy work to kernel thread]
        HARDWARE[Hardware Device<br>(Sensor / Peripheral / Controller)]
    end

    %% ===========
    %% CONNECTIONS
    %% ===========

    %% Control Path
    SYSFS --> ATTR --> DRIVER

    %% Data Path
    APP -->|syscalls| VFS --> CHAR_FILE --> DRIVER
    DRIVER --> HARDWARE
    HARDWARE -->|interrupt| ISR --> WORKQ
    WORKQ --> DRIVER
    DRIVER --> WAITQ
    WAITQ -->|wake up| APP

    %% Epoll notification path
    DRIVER -->|notify readiness| EPOLL[Epoll Events]
    EPOLL --> APP

    %% Group labels
    classDef dataPath fill:#e0f7fa,stroke:#0097a7,stroke-width:1px;
    classDef controlPath fill:#f1f8e9,stroke:#558b2f,stroke-width:1px;
    classDef kernel fill:#f5f5f5,stroke:#424242,stroke-width:1px;
    classDef user fill:#fff3e0,stroke:#ef6c00,stroke-width:1px;

    class USER_SPACE user;
    class KERNEL_SPACE kernel;
    class SYSFS,ATTR controlPath;
    class APP,VFS,CHAR_FILE,DRIVER,HARDWARE,ISR,WAITQ,WORKQ,EPOLL dataPath;
