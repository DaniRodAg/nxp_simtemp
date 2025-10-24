# Kernel module
obj-m += nxp_simtemp.o

# DTS files
DTS_SRC := ~/simtemp/kernel/dts/nxp-simtemp.dts
DTB := simtemp.dtbo

# Extra compiler flags (optional)
ccflags-y := -Wall -Wextra

# Kernel build directory
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Userspace program
USR_SRC := ~/simtemp/cli/main.c
USR_BIN := main_app

# Default target
all: $(DTB) $(USR_BIN) kernel

# Compile DTS
$(DTB): $(DTS_SRC)
	dtc -@ -I dts -O dtb -o $@ $<

# Build kernel module
kernel:
	make -C $(KDIR) M=$(PWD) modules

# Build userspace app
$(USR_BIN): $(USR_SRC)
	gcc -Wall -o $@ $<

# Clean everything
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(DTB) $(USR_BIN)
