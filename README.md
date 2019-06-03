# Arduino Support for Smart LEDs 2018-19

This repository contains scripts for VLC communication with Arduino microcontrollers. The sender is configured to send an initial program and the receiver is programmed to decode the VLC signal and transfer it via ICSP to a third microcontroller. The setup uses a Zigduino R2 sender, a Funduino (nano atmega328) receiver and a target Arduino (nano atmega328p) that gets programmed over VLC.

