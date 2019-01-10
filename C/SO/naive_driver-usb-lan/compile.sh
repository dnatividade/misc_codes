#!/bin/sh

sudo rmmod naive_usb -f
make clean
make
sudo insmod naive_usb.ko


