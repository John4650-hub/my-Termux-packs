#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "astyle.h"

// Function to read the content of a file
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Function to write content to a file
void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error writing to file: " << filename << std::endl;
        return;
    }
    file << content;
}

// AStyle callback function for error handling
void errorHandler(int errorNumber, const char* errorMessage) {
    std::cerr << "AStyle error: " << errorNumber << " - " << errorMessage << std::endl;
}

// Function to format the content using AStyle
std::string formatWithAStyle(const std::string& content) {
    char* options = (char*) "--style=google";
    char* formattedContent = AStyleMain(content.c_str(), options, errorHandler);
    if (!formattedContent) {
        std::cerr << "Error formatting content with AStyle" << std::endl;
        return "";
    }
    std::string result(formattedContent);
    delete[] formattedContent;
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string content = readFile(filename);
    if (content.empty()) {
        return 1;
    }

    std::string formattedContent = formatWithAStyle(content);
    if (formattedContent.empty()) {
        return 1;
    }

    writeFile(filename, formattedContent);
    std::cout << "File formatted successfully: " << filename << std::endl;
    return 0;
}
