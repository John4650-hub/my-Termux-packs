#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

int main() {
    std::ifstream puzzleDataHandle("../../../../input/testInput.txt");
    if (!puzzleDataHandle.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }


    std::vector<std::pair<int, int>> data;
    std::string line;

    // Read the file line by line
    while (std::getline(puzzleDataHandle, line)) {
        std::istringstream ss(line);
        std::string part1, part2;
        if (ss >> part1 >> part2) {
            data.emplace_back(std::stoi(part1), std::stoi(part2));
        }
    }

    puzzleDataHandle.close();

    std::vector<int> col1, col2;

    // Separate data into two columns
    for (const auto& pair : data) {
        col1.push_back(pair.first);
        col2.push_back(pair.second);
    }

    // Sort the columns
    std::sort(col1.begin(), col1.end());
    std::sort(col2.begin(), col2.end());

    int totalDistance = 0;

    // Calculate the total distance
    for (size_t i = 0; i < col1.size(); ++i) {
        totalDistance += std::abs(col1[i] - col2[i]);
    }

    std::cout << totalDistance << std::endl;

    return 0;
}
