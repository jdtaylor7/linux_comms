#include <iostream>
#include <string>
#include <thread>

// #include "serial.hpp"
#include <libserial/SerialStream.h>

const std::string SERIAL_PORT_1{"/dev/ttyACM0"};

int main()
{
    using namespace LibSerial;

    SerialStream serial_device;

    try
    {
        serial_device.Open(SERIAL_PORT_1.c_str());
    }
    catch (const OpenFailed&)
    {
        std::cerr << "Failed to open serial port\n";
        return 1;
    }

    serial_device.SetBaudRate(BaudRate::BAUD_9600) ;
    serial_device.SetCharacterSize(CharacterSize::CHAR_SIZE_8) ;
    serial_device.SetFlowControl(FlowControl::FLOW_CONTROL_NONE) ;
    serial_device.SetParity(Parity::PARITY_NONE) ;
    serial_device.SetStopBits(StopBits::STOP_BITS_1) ;

    while (1)
    {
        if (serial_device.IsDataAvailable())
        {
            using namespace std::chrono_literals;

            char recv_byte;

            serial_device.get(recv_byte);
            std::cout << recv_byte;

            std::this_thread::sleep_for(1us);
        }
    }

    return 0;
}
