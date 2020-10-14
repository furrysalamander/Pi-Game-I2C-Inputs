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

using namespace std;

const uint8_t BUTTON_COUNT = 16;
const uint8_t AXIS_COUNT = 4;

const int BUTTONS[BUTTON_COUNT - 1] = {
    BTN_A,         BTN_B,         BTN_X,          BTN_Y,      BTN_DPAD_UP,
    BTN_DPAD_DOWN, BTN_DPAD_LEFT, BTN_DPAD_RIGHT, BTN_TL,     BTN_TR,
    BTN_THUMBL,    BTN_THUMBR,    BTN_START,      BTN_SELECT, BTN_MODE
};

const int AXIS[AXIS_COUNT] {
    ABS_X, ABS_Y, ABS_RX, ABS_RY
};

struct rx_buffer_t
{
    uint8_t buttons[BUTTON_COUNT];
    uint16_t axis[AXIS_COUNT];
} rx_buffer;

int emit(int fd, int type, int code, int val)
{
    struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;
    return write(fd, &ie, sizeof(ie));
}

int main()
{
    int i2c_fd;
    i2c_fd = wiringPiI2CSetup(0x08); // Setup I2C Interface

    int gamepad_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); //opening of uinput
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
    uidev.absmax[ABS_X] = 512; // Max and min values
    uidev.absmin[ABS_X] = -512;
    uidev.absfuzz[ABS_X] = 0;  // Noise Filtering Parameter
    uidev.absflat[ABS_X] = 16; // Deadzone
    uidev.absmax[ABS_Y] = 512;
    uidev.absmin[ABS_Y] = -512;
    uidev.absfuzz[ABS_Y] = 0;
    uidev.absflat[ABS_Y] = 16;
    uidev.absmax[ABS_RX] = 512;
    uidev.absmin[ABS_RX] = -512;
    uidev.absfuzz[ABS_RX] = 0;
    uidev.absflat[ABS_RX] = 16;
    uidev.absmax[ABS_RY] = 512;
    uidev.absmin[ABS_RY] = -512;
    uidev.absfuzz[ABS_RY] = 0;
    uidev.absflat[ABS_RY] = 16;

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

    while (1)
    {
        read(i2c_fd, &rx_buffer, sizeof(rx_buffer_t)); // Read in the button and joystick status

        for (int i = 0; i < BUTTON_COUNT - 1; i++)
        {
            emit(gamepad_fd, EV_KEY, BUTTONS[i], rx_buffer.buttons[i]);
        }
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            emit(gamepad_fd, EV_ABS, AXIS[i], rx_buffer.axis[i]);
        }

        // emit(gamepad_fd, EV_KEY, BTN_X, 1);      // send that the X button is pushed
        // emit(gamepad_fd, EV_ABS, ABS_X, 256);    // send that the X axis is at 256
        emit(gamepad_fd, EV_SYN, SYN_REPORT, 0); // send a synchronize report, to signify that this is the last of the data for this event

        usleep(5000);
    }

    if (ioctl(gamepad_fd, UI_DEV_DESTROY) < 0)
    {
        printf("error: ioctl");
        return 1;
    }

    close(gamepad_fd);
    return 1;
}