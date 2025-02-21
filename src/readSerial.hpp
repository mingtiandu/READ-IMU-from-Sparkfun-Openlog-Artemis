#ifndef READSERIAL_HPP
#define READSERIAL_HPP

#include <string>
#include <vector>
#include <windows.h>

// Function to set up the serial port
HANDLE setupSerial(const std::string& portName);

// Function to read a raw string line from the serial port
std::string readSerialLine(HANDLE hSerial);

// Function to extract aX and aY from a comma-separated string
std::vector<float> extractAxAy(const std::string& line);

#endif // READSERIAL_HPP#pragma once
