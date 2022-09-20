#!/bin/sh

#Add Modules to Linux Kernel
modprobe i2c-dev
modprobe i2c-bcm2835
modprobe rtc-ds1307
echo ds1307 0x68 >> sys/class/i2c-adapter/i2c-1/new_device
hwclock -s

#Start Device Drivers
insmod /etc/SMD/DDrivers/buzz.ko
insmod /etc/SMD/DDrivers/pwmPinDriver01.ko

#Start Daemons
/etc/SMD/Daemons/fingerprint_daemon.elf
/etc/SMD/Daemons/rfid_daemon.elf

#Start QML Application
/etc/SMD/qml &