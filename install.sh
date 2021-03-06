#!/bin/bash

DRIVERS="~\/fbcp-ili9312 \&\n\~\/Pi-Game-I2C-Inputs\/i2c_driver \&\n"

apt-get install cmake

#cd ~
#git clone https://github.com/furrysalamander/Pi-Game-I2C-Inputs.git
#cd Pi-Game-I2C-Inputs
curl -sS https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/i2samp.sh | bash

apt-get install wiringpi
g++ main.cpp -lwiringPi -o i2c_driver
./i2c_driver &

cd ~
git clone https://github.com/furrysalamander/fbcp-ili9341.git
cd fbcp-ili9341

mkdir build
cd build
cmake -DST7796S=ON -DGPIO_TFT_DATA_CONTROL=25 -DGPIO_TFT_RESET_PIN=27 -DGPIO_TFT_BACKLIGHT=24 -DSPI_BUS_CLOCK_DIVISOR=4 -DDISPLAY_SWAP_BGR=ON -DBACKLIGHT_CONTROL=ON ..
make -j
./fbcp-ili9341 &

sed -i "s/$DRIVERS//g" /etc/rc.local

sed -i "s/\nexit 0/$DRIVERS&/g" /etc/rc.local

sed -i "s/#disable_overscan/disable_overscan/g" /boot/config.txt
sed -i "s/#dtparam=i2c_arm/dtparam=i2c_arm/g" /boot/config.txt
