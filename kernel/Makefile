# Object file to build
obj-m += nxp_simtemp.o

# Extra compiler flags (optional)
ccflags-y := -Wall -Wextra

# Build rules
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

