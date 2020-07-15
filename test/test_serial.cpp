#include <iostream>
#include <string>
#include <thread>

#include "bounded_buffer.hpp"
#include "serial.hpp"

const std::string SERIAL_PORT_1{"/dev/ttyACM0"};

int main()
{
    using namespace LibSerial;

    /*
     * Create serial device.
     */
    SerialDeviceConfig cfg{
        BaudRate::BAUD_9600,
        CharacterSize::CHAR_SIZE_8,
        FlowControl::FLOW_CONTROL_NONE,
        Parity::PARITY_NONE,
        StopBits::STOP_BITS_1};
    auto buf = std::make_shared<BoundedBuffer<char>>(1024);
    SerialDevice dev{buf};

    /*
     * Open device.
     */
    if (!dev.open(SERIAL_PORT_1)) { return 1; }

    /*
     * Configure device.
     */
    dev.config(cfg);

    /*
     * Read data from device into buffer.
     */
    dev.start_reading();

    /*
     * Process data from buffer.
     */
    while (1)
    {
        auto recv_byte_p = buf->try_pop();
        if (recv_byte_p)
            std::cout << *recv_byte_p;
    }

    return 0;
}
