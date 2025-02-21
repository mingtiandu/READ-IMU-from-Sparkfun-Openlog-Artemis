#include "readSerial.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>

// Function to set up the serial port
HANDLE setupSerial(const std::string& portName) {
    // Open the serial port
    HANDLE hSerial = CreateFile(
        portName.c_str(), // Convert std::string to LPCSTR
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Error opening serial port!");
    }

    // Configure serial port settings
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        throw std::runtime_error("Error getting serial port state!");
    }

    dcbSerialParams.BaudRate = CBR_115200; // Set baud rate to 115200
    dcbSerialParams.ByteSize = 8;        // 8 data bits
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 stop bit
    dcbSerialParams.Parity = NOPARITY;  // No parity

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        throw std::runtime_error("Error setting serial port state!");
    }

    // Set timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        CloseHandle(hSerial);
        throw std::runtime_error("Error setting timeouts!");
    }

    // Flush the serial port
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    return hSerial;
}

// Function to read a raw string line from the serial port
std::string readSerialLine(HANDLE hSerial) {
    char buffer[512]; // Buffer to store incoming data
    DWORD bytesRead;
    static std::string line; // Persistent line buffer

    while (true) {
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0'; // Null-terminate the string
                line += buffer;           // Append to the line buffer

                // Check if a complete line has been received
                size_t pos;
                if ((pos = line.find('\n')) != std::string::npos) {
                    std::string completeLine = line.substr(0, pos); // Extract the line
                    line.erase(0, pos + 1);                       // Remove the processed line
                    return completeLine; // Return the complete line
                }
            }
        }
        else {
            throw std::runtime_error("Error reading from serial port!");
        }
    }
}

// Function to extract aX and aY from a comma-separated string
std::vector<float> extractAxAy(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;

    // Split the line by commas
    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }

    // Check if the line has enough tokens
    if (tokens.size() < 4) {
        throw std::runtime_error("Invalid line format: Not enough tokens");
    }

    std::vector<float> result;

    // Extract aX (3rd token, index 2)
    try {
        result.push_back(std::stof(tokens[2]));
    }
    catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid aX value: Could not convert to float");
    }

    // Extract aY (4th token, index 3)
    try {
        result.push_back(std::stof(tokens[3]));
    }
    catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid aY value: Could not convert to float");
    }

    return result;
}
