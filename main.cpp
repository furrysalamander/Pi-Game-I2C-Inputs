#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>

const uint8_t BUTTON_COUNT = 16;
const uint8_t AXIS_COUNT = 4;

struct rx_buffer_t {
  uint8_t buttons[BUTTON_COUNT];
  uint16_t axis[AXIS_COUNT];
};

uint8_t rx_buffer_tmp[sizeof(rx_buffer_t)];

using namespace std;

int main()
{
    int fd;

    fd = wiringPiI2CSetup(0x08);

    cout << "Starting Test..." << endl;

    while(true)
    {
        for (int i = 0; i < sizeof(rx_buffer_t); i++)
        {
            rx_buffer_tmp[i] = wiringPiI2CRead(fd);
        }
        rx_buffer_t rx_buffer = *((rx_buffer_t*)rx_buffer_tmp);
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            cout << rx_buffer.buttons[i];
        }
        cout << endl;
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            cout << rx_buffer.buttons[i];
        }
        cout << endl;
    }

    return 0;
}