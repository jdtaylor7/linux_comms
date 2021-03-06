#include <algorithm>
#include <array>
#include <iostream>

#include "usb.hpp"

#include "bounded_buffer.hpp"

int main()
{
    libusb_init(nullptr);

    libusb_device** libusb_devs;
    std::size_t dev_count;

    dev_count = libusb_get_device_list(nullptr, &libusb_devs);
    if (dev_count < 0)
    {
        std::cout << "No devices found\n";
        return 1;
    }

    /*
     * List connected USB devices.
     */
    libusb_device* libusb_dev;
    libusb_device_handle* libusb_handle;
    int i = 0;
    std::array<unsigned char, 128> summary;

    std::cout << "\nThese usb devices are currently detected:\n";
    while ((libusb_dev = libusb_devs[i++]) != nullptr)
    {
        struct libusb_device_descriptor desc;

        int r = libusb_get_device_descriptor(libusb_dev, &desc);
        if (r < 0)
            std::cerr << "Couldn't get device descriptor\n";

        int status = libusb_open(libusb_dev, &libusb_handle);
        if (status < 0)
        {
            std::cerr << "libusb_open failed with " << status << '\n';
        }
        else
        {
            std::string m_str;
            std::string p_str;
            summary.fill('\0');

            std::cout << "device " << i - 1 << ": ";

            r = libusb_get_string_descriptor_ascii(libusb_handle, desc.iManufacturer, summary.data(), summary.size());
            m_str.assign(std::begin(summary), std::find(std::begin(summary), std::end(summary), static_cast<unsigned char>('\0')));
            if (m_str == "")
                std::cout << "<Manufacturer not found>";
            else
                std::cout << m_str;

            std::cout << " | ";

            r = libusb_get_string_descriptor_ascii(libusb_handle, desc.iProduct, summary.data(), summary.size());
            p_str.assign(std::begin(summary), std::find(std::begin(summary), std::end(summary), static_cast<unsigned char>('\0')));
            if (p_str == "")
                std::cout << "<Product not found>";
            else
                std::cout << p_str;

            std::cout << '\n';
            libusb_close(libusb_handle);
        }
    }

    /*
     * Select USB device to use.
     */
    int choice = -1;
    while ((choice < 0) || (choice > i - 2))
    {
        std::cout << "Select a device (0 - " << i - 2 << "): ";
        std::cin >> choice;
    }
    std::cout << '\n';

    libusb_dev = libusb_devs[choice];

    if (libusb_dev == nullptr)
    {
        std::cerr << "Device is null\n";
        return 1;
    }

    // ********** TODO remove
    struct libusb_device_descriptor desc;

    int r = libusb_get_device_descriptor(libusb_dev, &desc);
    if (r < 0)
        std::cerr << "Couldn't get device descriptor\n";

    int status = libusb_open(libusb_dev, &libusb_handle);
    if (status < 0)
    {
        std::cerr << "libusb_open failed with " << status << '\n';
    }
    else
    {
        std::string m_str;
        std::string p_str;
        summary.fill('\0');

        std::cout << "device " << choice << ": ";

        r = libusb_get_string_descriptor_ascii(libusb_handle, desc.iManufacturer, summary.data(), summary.size());
        m_str.assign(std::begin(summary), std::find(std::begin(summary), std::end(summary), static_cast<unsigned char>('\0')));
        std::cout << m_str << " | ";

        r = libusb_get_string_descriptor_ascii(libusb_handle, desc.iProduct, summary.data(), summary.size());
        p_str.assign(std::begin(summary), std::find(std::begin(summary), std::end(summary), static_cast<unsigned char>('\0')));
        std::cout << p_str;

        std::cout << '\n';
        libusb_close(libusb_handle);
    }
    // ********** TODO remove

    /*
     * Remove references to other devices.
     */
    libusb_free_device_list(libusb_devs, 1);

    /*
     * Create bounded bufer.
     */
    auto buf = std::make_shared<BoundedBuffer<unsigned char>>(1024);

    /*
     * Get device handle.
     */
    status = libusb_open(libusb_dev, &libusb_handle);
    if (status < 0)
    {
        std::cerr << "libusb_open failed with " << status << '\n';
    }
    else
    {
        /*
         * Get endpoint descriptor.
         */
        struct libusb_endpoint_descriptor endpoint_desc;
        std::array<unsigned char, 1> endpoint_desc_data;
        int r = libusb_get_descriptor(
            libusb_handle,
            LIBUSB_DT_ENDPOINT,
            endpoint_desc.bEndpointAddress,
            endpoint_desc_data.data(),
            endpoint_desc_data.size());

        unsigned char endpoint_desc_address = endpoint_desc_data[0];

        std::cout << "endpoint_desc_address1: " << static_cast<unsigned int>(endpoint_desc_address) << '\n';
        std::cout << "endpoint_desc_address2: " << static_cast<unsigned int>(endpoint_desc.bEndpointAddress) << '\n';

        /*
         * Read from device.
         */
        while (1)
        {
            int bytes_transferred = 0;
            int tmp_buf_size = 1024;
            unsigned char tmp_buf[tmp_buf_size];

            libusb_interrupt_transfer(
                libusb_handle,          // usb handle
                endpoint_desc_address,  // endpoint
                tmp_buf,                // recv buf
                tmp_buf_size,           // size of recv buf
                &bytes_transferred,     // number bytes transferred
                1);                     // 1ms timeout

            for (i = 0; i < bytes_transferred; i++)
            {
                std::cout << tmp_buf[i];
            }
        }
    }

    libusb_exit(nullptr);

    return 0;
}
