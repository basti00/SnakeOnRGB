cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME ard)
project(${PROJECT_NAME})

set(ARDUINO_DEFAULT_PORT /dev/cu.usbmodemfd121)

set(${CMAKE_PROJECT_NAME}_SKETCH ard.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})