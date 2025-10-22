```mermaid
flowchart LR
    subgraph KERNEL_SPACE[Kernel Space]
        SYSFS[SYSFS<br>stores drivers info]
        ATTRIBUTES[SYSFS attributes<br>store driver controls]
        FILE_OPS[File Operation System]
        CHAR_FILE[Character dev file<br>stores info in a virtual file]
        CHAR_DRIVER[Character dev driver<br>sends I/O read requests]
        WAITQ[Wait Queue]
        WORKQ[Work Queue]
    end

    subgraph USER_SPACE[User Space]
        USER[User Space Application]
    end

    SYSFS --> ATTRIBUTES --> FILE_OPS
    FILE_OPS --> CHAR_FILE --> CHAR_DRIVER
    CHAR_DRIVER --> WAITQ
    WAITQ --> WORKQ
    WORKQ --> WAITQ
    WAITQ --> EPOLL[Epoll Events]
    EPOLL --> USER
