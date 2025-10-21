# Object file to build
obj-m += nxp_simtemp.o

# Extra compiler flags (optional)
ccflags-y := -Wall -Wextra

# Build rules
all:
	gcc -o main_app ~/simtemp/cli/main.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

