#include "serial.hpp"

SerialDevice::SerialDevice(
        std::shared_ptr<BoundedBuffer<char>> byte_buffer_) :
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
    stream.SetBaudRate(br);
    stream.SetCharacterSize(cs);
    stream.SetFlowControl(fc);
    stream.SetParity(p);
    stream.SetStopBits(sb);
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
            stream.get(recv_byte);
            byte_buffer->force_push(recv_byte);

            std::this_thread::sleep_for(1us);
        }
    }
}

void SerialDevice::stop_reading()
{
    keep_reading.store(false);
}
