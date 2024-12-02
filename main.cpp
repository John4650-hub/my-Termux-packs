#include <iostream>
#include <string>
#include <ctime>

int main() {
    std::string name;
    int birthYear, birthMonth, birthDay;

    // Get the current date
    time_t t = time(0);
    tm* now = localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;

    // Ask for user's name
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    // Ask for user's birthday
    std::cout << "Enter your birth year (e.g., 1990): ";
    std::cin >> birthYear;
    std::cout << "Enter your birth month (1-12): ";
    std::cin >> birthMonth;
    std::cout << "Enter your birth day (1-31): ";
    std::cin >> birthDay;

    // Calculate the age
    int age = currentYear - birthYear;
    if (currentMonth < birthMonth || (currentMonth == birthMonth && currentDay < birthDay)) {
        age--;
    }

    // Wishing the user
    std::cout << "Hello, " << name << "! ";
    std::cout << "Wishing you a happy " << age << "th birthday this year!" << std::endl;

    return 0;
}
