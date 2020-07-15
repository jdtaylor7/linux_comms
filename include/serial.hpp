#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "libserial/SerialStream.h"
#include "bounded_buffer.hpp"

struct SerialDeviceConfig
{
    SerialDeviceConfig(
            LibSerial::BaudRate br_,
            LibSerial::CharacterSize cs_,
            LibSerial::FlowControl fc_,
            LibSerial::Parity py_,
            LibSerial::StopBits sb_) :
        br(br_), cs(cs_), fc(fc_), py(py_), sb(sb_) {}

    LibSerial::BaudRate br;
    LibSerial::CharacterSize cs;
    LibSerial::FlowControl fc;
    LibSerial::Parity py;
    LibSerial::StopBits sb;
};

class SerialDevice
{
public:
    SerialDevice(std::shared_ptr<BoundedBuffer<char>>);

    bool open(const std::string&);
    void config(const SerialDeviceConfig&);

    bool is_open() const;
    bool is_reading() const;

    void start_reading();
    void stop_reading();
private:
    LibSerial::SerialStream stream;
    std::string port_name{};

    std::shared_ptr<BoundedBuffer<char>> byte_buffer;
    std::atomic<bool> port_open = false;
    std::atomic<bool> keep_reading = false;
};

#endif /* SERIAL_HPP */
