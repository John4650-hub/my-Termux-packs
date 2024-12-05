#include <iostream>
#include <ctime>

int main() {
    // Get the current time
    std::time_t currentTime = std::time(nullptr);

    // Convert the time to a readable format
    std::tm* localTime = std::localtime(&currentTime);

    // Print the current date and time
    std::cout << "Current Date and Time: "
              << (localTime->tm_year + 1900) << "-" // Year
              << (localTime->tm_mon + 1) << "-"    // Month
              << localTime->tm_mday << " "        // Day
              << localTime->tm_hour << ":"        // Hour
              << localTime->tm_min << ":"         // Minute
              << localTime->tm_sec                // Second
              << std::endl;

    return 0;
}
