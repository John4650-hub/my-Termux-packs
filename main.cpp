#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include "/usr/include/nlohmann/json.hpp"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

// Function to determine file extension and set icon based on a mapping
string getFileIcon(const string& filename, const unordered_map<string, string>& extToIcon) {
    string ext;
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != string::npos) {
        ext = filename.substr(dotPos + 1);
    }

    auto it = extToIcon.find(ext);
    return it != extToIcon.end() ? it->second : "fa fa-file";
}

// Function to list directory contents recursively, building a JSON structure
vector<json> listDir(const string& url, const unordered_map<string, string>& extToIcon, int maxDepth = 10) {
    string path = url.empty() ? fs::current_path().string() : url + "/";

    vector<json> result;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry) && maxDepth > 0) {
                json dirEntry;
                dirEntry["text"] = entry.path().filename().string();
                dirEntry["nativeURL"] = entry.path().string();
                dirEntry["state"] = {{"checked", false}, {"expanded", false}, {"selected", false}};
                dirEntry["nodes"] = listDir(entry.path().string(), extToIcon, maxDepth - 1);
                result.push_back(dirEntry);
            } else {
                json fileEntry;
                fileEntry["text"] = entry.path().filename().string();
                fileEntry["nativeURL"] = entry.path().string();
                fileEntry["onclick"] = "getUrls(this)";
                fileEntry["icon"] = getFileIcon(entry.path().filename().string(), extToIcon);
                result.push_back(fileEntry);
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return result;
}

// Function to load file contents (assuming folder.txt contains the folder path)

string loadFs() {
    string folderPath;
    ifstream folderFile("/storage/emulated/0/.Apps/Ace-Code/folder.txt");

    if (!folderFile.is_open()) {
        cerr << "Error: Could not open folder.txt" << endl;
        return "";
    }

    getline(folderFile, folderPath);
    
    // Safely remove trailing newlines
    size_t pos = folderPath.find_last_of("\n\r");
    if (pos != string::npos) {
        folderPath.erase(pos);
    }

    unordered_map<string, string> extToIcon = {
        {"py", "fab fa-python"},
        {"java", "fab fa-java"},
        {"html", "fab fa-html5"},
        {"css", "fab fa-css3"},
        {"js", "fab fa-js"},
        {"c", "fab fa-cuttlefish"},
        // Add more extensions to the mapping here
    };

    vector<json> data = listDir(folderPath, extToIcon);

    json result;
    result["data"] = data;

    try {
        return result.dump(2);
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return "";
    }
}


int main() {
    string jsonData = loadFs();
    if (!jsonData.empty()) {
        cout << jsonData << endl;
    }
    return 0;
}
