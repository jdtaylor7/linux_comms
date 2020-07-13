cmake_minimum_required(VERSION 3.14)

# Set project name.
project(linux_comms)

# Specify C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create compilation database for Sourcetrail.
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Find libraries.
find_library(LIBSERIAL_LIB serial "${PROJECT_SOURCE_DIR}/third_party/libserial/lib")

# Add include directories
include_directories(
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/third_party/libserial/include"
    "${PROJECT_SOURCE_DIR}/third_party/bounded_buffer/include"
    "/usr/include/libusb-1.0"
)
message(STATUS "include dirs:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "    ${dir}")
endforeach()

# Add compiler/linker options.
set(LINKOPTS "${LINKOPTS} -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXOPTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKOPTS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# Add libraries.
add_library(serial OBJECT src/serial.cpp)
add_library(usb OBJECT src/usb.cpp)

# Add executables.
add_executable(test_serial test/test_serial.cpp)
add_executable(test_usb test/test_usb.cpp)

# Link libraries.
target_link_libraries(test_serial serial "${LIBSERIAL_LIB}")
target_link_libraries(test_usb usb usb-1.0)
