#!/bin/sh

IF_TUN=tun0
BAUD_RATE=9600
DEVICE="/dev/ttyUSB0"

##to_exec ./serial-tun -i <interface name> -p <serial port> -b <baud rate>
sudo ~/Ne2Ino/Serial-TUN/serial_tun -i $IF_TUN -b $BAUD_RATE -p /dev/ttyUSB0

