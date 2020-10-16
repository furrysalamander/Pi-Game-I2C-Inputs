#pragma once

#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

int init_gamepad(int& gamepad_fd) {
        if (gamepad_fd < 0)
    {
        printf("Opening of uinput failed!\n");
        return 1;
    }
    ioctl(gamepad_fd, UI_SET_EVBIT, EV_KEY); //setting Gamepad keys
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_A);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_B);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_X);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_Y);

    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_TL);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_TR);

    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_START);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_SELECT);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_MODE);

    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_THUMBL);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_THUMBR);

    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_DPAD_UP);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
    ioctl(gamepad_fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);

    ioctl(gamepad_fd, UI_SET_EVBIT, EV_ABS); //setting Gamepad thumbsticks
    ioctl(gamepad_fd, UI_SET_ABSBIT, ABS_X);
    ioctl(gamepad_fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(gamepad_fd, UI_SET_ABSBIT, ABS_RX);
    ioctl(gamepad_fd, UI_SET_ABSBIT, ABS_RY);

    struct uinput_user_dev uidev; //setting the default settings of Gamepad

    memset(&uidev, 0, sizeof(uidev)); // fill the memory where uidev is located with zeros

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Integrated Gamepad"); //Name of Gamepad

    uidev.id.bustype = BUS_USB; // Not sure if this stuff is necessary
    uidev.id.vendor = 0x3;
    uidev.id.product = 0x3;
    uidev.id.version = 2;

    // Parameters for the joysticks.
    // These have to be set for both axes on both joysticks.
    uidev.absmax[ABS_X] = 1023; // Max and min values
    uidev.absmin[ABS_X] = 0;
    uidev.absfuzz[ABS_X] = 0;  // Noise Filtering Parameter
    uidev.absflat[ABS_X] = 64; // Deadzone
    uidev.absmax[ABS_Y] = 1023;
    uidev.absmin[ABS_Y] = 0;
    uidev.absfuzz[ABS_Y] = 0;
    uidev.absflat[ABS_Y] = 64;
    uidev.absmax[ABS_RX] = 1023;
    uidev.absmin[ABS_RX] = 0;
    uidev.absfuzz[ABS_RX] = 0;
    uidev.absflat[ABS_RX] = 64;
    uidev.absmax[ABS_RY] = 1023;
    uidev.absmin[ABS_RY] = 0;
    uidev.absfuzz[ABS_RY] = 0;
    uidev.absflat[ABS_RY] = 64;

    if (write(gamepad_fd, &uidev, sizeof(uidev)) < 0) //writing settings
    {
        printf("error: write");
        return 1;
    }
    if (ioctl(gamepad_fd, UI_DEV_CREATE) < 0) //writing ui dev create
    {
        printf("error: ui_dev_create");
        return 1;
    }
    return 0;
}