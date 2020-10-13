#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <unistd.h>

const uint8_t BUTTON_COUNT = 16;
const uint8_t AXIS_COUNT = 4;

struct rx_buffer_t {
  uint8_t buttons[BUTTON_COUNT];
  uint16_t axis[AXIS_COUNT];
};

using namespace std;

int main()
{
    int fd;

    fd = wiringPiI2CSetup(0x08);

    while(true)
    {
        cout << "\033[2J\033[1;1H";
        cout << "Starting Test..." << endl;

        rx_buffer_t rx_buffer;
        read(fd, &rx_buffer, sizeof(rx_buffer_t));

        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            cout << (int)rx_buffer.buttons[i];
        }
        cout << endl;
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            cout << rx_buffer.axis[i] << ' ';
        }
        cout << endl;
        usleep(500000);
    }

    return 0;
}