#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "libserial/SerialStream.h"
#include "bounded_buffer.hpp"

class SerialDevice
{
public:
    SerialDevice(std::shared_ptr<BoundedBuffer<char>>);
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

    std::shared_ptr<BoundedBuffer<char>> byte_buffer;
    std::atomic<bool> keep_reading = false;
};

#endif /* SERIAL_HPP */
