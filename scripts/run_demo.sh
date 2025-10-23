# Insert the module
insmod ../kernel/nxp_simtemp.ko || exit 1
dmesg | tail -n 1

#!/bin/bash
#gnome-terminal -- bash -c "../kernel/main_app"

gnome-terminal -- bash -c "python3 ../user/gui/app.py; exec bash"


# Read parameters inside sys/class/misc/simtemp
echo "simtemp parameters"
echo "Mode: "
cat /sys/class/misc/simtemp/mode
echo "Threshold in mdC: "
cat /sys/class/misc/simtemp/threshold_mC
echo "Sampling time in ms: "
cat /sys/class/misc/simtemp/sampling_ms

#dmesg | tail

echo "Waiting for 20 seconds..."
sleep 6

echo "Reading data written inside /dev/simtemp"
#cat /dev/simtemp

#python3 ../user/gui/app.py

#dmesg | tail

echo "Modifying parameters"
echo "Changing mode to normal"
echo "NORMAL" > /sys/class/misc/simtemp/mode
cat /sys/class/misc/simtemp/mode

echo "Lowering the temperature threshold to 27 C"
echo 27000 >  /sys/class/misc/simtemp/threshold_mC
cat /sys/class/misc/simtemp/threshold_mC

echo "Setting sampling time to 500ms"
echo 500 >  /sys/class/misc/simtemp/sampling_ms
cat /sys/class/misc/simtemp/sampling_ms

echo "Wait for 2 seconds"
sleep 2

echo "Setting mode to ramp"
echo "RAMP" > /sys/class/misc/simtemp/mode
#cat /sys/class/misc/simtemp/mode

#dmesg | tail

echo "Waiting for 11 seconds..."
sleep 11

#python3 ../user/gui/app.py

#cat /dev/simtemp

echo "Changing mode to normal"
echo "NORMAL" > /sys/class/misc/simtemp/mode
cat /sys/class/misc/simtemp/mode

echo "Lowering the temperature threshold to 40 C"
echo 40000 >  /sys/class/misc/simtemp/threshold_mC
cat /sys/class/misc/simtemp/threshold_mC

echo "Setting mode to ramp"
echo "RAMP" > /sys/class/misc/simtemp/mode
cat /sys/class/misc/simtemp/mode

sleep 2

#cat /dev/simtemp

sleep 2

#dmesg | tail

read -n 1 -s

#pkill main_app
rmmod ../kernel/nxp_simtemp.ko

dmesg | tail
