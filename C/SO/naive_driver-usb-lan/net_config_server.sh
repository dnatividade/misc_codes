#!/bin/sh

USB_NAIVE="naive0"
IF_NAIVE="NaIVE0"
IP_MASK="10.10.10.10/24"

##Add an IP address to the first computer ...
sudo ip addr add $IP_MASK dev $IF_NAIVE
sudo ifconfig $IF_NAIVE up

sudo ifconfig $IF_NAIVE
##to_exec ./serial-tun -i <interface name> -p <serial port> -b <baud rate>
#sudo ~/Ne2Ino/Serial-TUN/serial_tun -i tun0 -b 9600 -p /dev/ttyUSB0

##Change permission for USB device
sudo chmod 666 /dev/$USB_NAIVE
