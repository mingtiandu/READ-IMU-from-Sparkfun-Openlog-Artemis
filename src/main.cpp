#include <iostream>
#include "readSerial.hpp"

#include <filesystem> // Filesystem create directory
#include <fstream> // Filestream write data

int main() {
    std::ofstream myfile;
    // Buffers to hold formatted date and time strings
    char dateBuffer[80];
    char timeBuffer[80];
    // Format time and date into the buffers
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm now_tm = *std::localtime(&now_c);
    strftime(timeBuffer, 80, "%Y-%m-%d-%H-%M-%S", &now_tm);
    strftime(dateBuffer, 80, "%Y-%m-%d", &now_tm);
    // Construct the path and name using the buffers
    std::string filePath = "./Data/" + std::string(dateBuffer) + "/";
    std::string myfileName = filePath + std::string(timeBuffer) + ".csv";
    // Create directories if they do not exist
    std::filesystem::create_directories(filePath);
    // Open the file for writing
    myfile.open(myfileName);
    myfile << "ax, ay" << std::endl;

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
                // Write aX and aY into my file if they are accessible
                myfile << std::setprecision(4) << aX << "," << aY << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error extracting aX/aY: " << e.what() << std::endl;
            }
        }

        // Close the serial port (this will never be reached in this example)
        CloseHandle(hSerial);
        // Close the file
        myfile.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}