#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "libserial/SerialStream.h"
#include "bounded_buffer.hpp"

const std::string SERIAL_PORT = "/dev/ttyACM0";

class SerialDevice
{
public:
    SerialDevice(std::shared_ptr<BoundedBuffer<unsigned char>>);
    bool open(const std::string&);
    void config(
        const LibSerial::BaudRate,
        const LibSerial::CharacterSize,
        const LibSerial::FlowControl,
        const LibSerial::Parity,
        const LibSerial::StopBits);
    bool is_data_available();
    void start_reading();
    void stop_reading();
private:
    LibSerial::SerialStream stream;
    std::string serial_port;

    std::shared_ptr<BoundedBuffer<unsigned char>> byte_buffer;
    std::atomic<bool> keep_reading = false;
};

SerialDevice::SerialDevice(
        std::shared_ptr<BoundedBuffer<unsigned char>> byte_buffer_) :
    stream{},
    byte_buffer{byte_buffer_}
{
}

bool SerialDevice::open(const std::string& port_name)
{
    try
    {
        stream.Open(port_name.c_str());
    }
    catch (const LibSerial::OpenFailed&)
    {
        std::cerr << "Failed to open serial port: " << port_name << '\n';
        return false;
    }
    return true;
}

void SerialDevice::config(
    const LibSerial::BaudRate br,
    const LibSerial::CharacterSize cs,
    const LibSerial::FlowControl fc,
    const LibSerial::Parity p,
    const LibSerial::StopBits sb
)
{

}

bool SerialDevice::is_data_available()
{
    return stream.IsDataAvailable();
}

void SerialDevice::start_reading()
{
    keep_reading.store(true);

    while (1)
    {
        if (keep_reading.load() && is_data_available())
        {
            using namespace std::chrono_literals;

            char recv_byte;
            byte_buffer->force_push(stream.get(recv_byte));

            std::this_thread::sleep_for(1us);
        }
    }
}

void SerialDevice::stop_reading()
{
    keep_reading.store(false);
}

#endif /* SERIAL_HPP */
