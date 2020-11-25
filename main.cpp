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

#include "gamepad_config.h"

using namespace std;

const uint8_t BUTTON_COUNT = 16;
const uint8_t AXIS_COUNT = 4;

const int POLLING_DELAY = 2000; // Decrease this value to increase the polling speed

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
    int16_t axis[AXIS_COUNT];
} rx_buffer, previous_state;

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

    if (init_gamepad(gamepad_fd)) {
        return 1;
    }

    bool needs_update = false;

    while (1)
    {
        read(i2c_fd, &rx_buffer, sizeof(rx_buffer_t)); // Read in the button and joystick status
        cout << '\r';
        for (int i = 0; i < BUTTON_COUNT - 1; i++)
        {
            // We only want to bother sending an update if something actually changes.
            if (previous_state.buttons[i] != rx_buffer.buttons[i])
            {
                previous_state.buttons[i] = rx_buffer.buttons[i];
                needs_update = true;
                emit(gamepad_fd, EV_KEY, BUTTONS[i], rx_buffer.buttons[i]); // send the event for each button
            }
            cout << static_cast<int>(rx_buffer.buttons[i]);
        }
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            // We only want to bother sending an update if something actually changes.
            if (previous_state.axis[i] != rx_buffer.axis[i])
            {
                previous_state.axis[i] = rx_buffer.axis[i];
                needs_update = true;
                emit(gamepad_fd, EV_ABS, AXIS[i], rx_buffer.axis[i]); // send the event for each axis
            }
        }
        if (needs_update)
        {
            emit(gamepad_fd, EV_SYN, SYN_REPORT, 0); // send a synchronize report, to signify that this is the last of the data for this event
        }
        cout << flush;
        usleep(POLLING_DELAY);
    }

    if (ioctl(gamepad_fd, UI_DEV_DESTROY) < 0)
    {
        printf("error: ioctl");
        return 1;
    }

    close(gamepad_fd);
    return 1;
}