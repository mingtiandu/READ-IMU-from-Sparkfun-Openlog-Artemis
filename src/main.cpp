#include <iostream>
#include "readSerial.hpp"

int main() {
    try {
        // Set up the serial port
        HANDLE hSerial = setupSerial("COM5");

        // Continuously read and process data
        while (true) {
            std::string line = readSerialLine(hSerial);
            std::cout << "Received: " << line << std::endl;

            // Extract and print aX and aY
            try {
                std::vector<float> axAy = extractAxAy(line);
                float aX = axAy[0];
                float aY = axAy[1];
                std::cout << "aX: " << aX << ", aY: " << aY << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error extracting aX/aY: " << e.what() << std::endl;
            }
        }

        // Close the serial port (this will never be reached in this example)
        CloseHandle(hSerial);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}