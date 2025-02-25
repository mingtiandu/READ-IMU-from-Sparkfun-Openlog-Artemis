# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.12)

# Set the version of C++ compiler
SET (CMAKE_CXX_STANDARD 20)

# Set the project name
project(read_imu_cpp)

# Add the executable target
add_executable(ReadIMU ../src/main.cpp ../src/readSerial.cpp)