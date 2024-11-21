#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

// Utility function to check if a path is a directory
bool isDirectory(const string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false; // Cannot access path
    }
    return (info.st_mode & S_IFDIR) != 0;
}

// Function to determine file extension and set icon based on a mapping
string getFileIcon(const string& filename, const map<string, string>& extToIcon) {
    string ext;
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != string::npos) {
        ext = filename.substr(dotPos + 1);
    }

    auto it = extToIcon.find(ext);
    return it != extToIcon.end() ? it->second : "fa fa-file text-primary";
}

// Function to list directory contents recursively, building a JSON-like structure
vector<map<string, string>> listDir(const string& path, const map<string, string>& extToIcon, int maxDepth = 10) {
    vector<map<string, string>> result;

    if (maxDepth <= 0) {
        return result;
    }

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        cerr << "Error: Could not open directory " << path << endl;
        return result;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string entryName = entry->d_name;

        // Skip "." and ".."
        if (entryName == "." || entryName == "..") {
            continue;
        }

        string fullPath = path + "/" + entryName;

        if (isDirectory(fullPath)) {
            // Recursive call for subdirectories
            map<string, string> dirEntry;
            dirEntry["text"] = entryName;
            dirEntry["nativeURL"] = fullPath;
            dirEntry["type"] = "directory";

            stringstream state;
            state << "{\"checked\": false,\"expanded\": false,\"selected\": false}";
            dirEntry["state"] = state.str();

            vector<map<string, string>> nodes = listDir(fullPath, extToIcon, maxDepth - 1);
            if (!nodes.empty()) {
                stringstream ss;
                for (const auto& child : nodes) {
                    ss << child.at("text") << ",";
                }
                dirEntry["nodes"] = ss.str(); // Placeholder for child nodes
            }

            result.push_back(dirEntry);
        } else {
            // Add file entry
            map<string, string> fileEntry;
            fileEntry["text"] = entryName;
            fileEntry["nativeURL"] = fullPath;
            fileEntry["onclick"] = "getUrls(this)";
            fileEntry["icon"] = getFileIcon(entryName, extToIcon);

            result.push_back(fileEntry);
        }
    }

    closedir(dir);
    return result;
}

// Function to load file system data (assuming folder.txt contains the folder path)
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

    map<string, string> extToIcon = {
        {"py", "fab fa-python"},
        {"java", "fab fa-java"},
        {"html", "fab fa-html5"},
        {"css", "fab fa-css3"},
        {"js", "fab fa-js"},
        {"c", "fab fa-cuttlefish"},
        // Add more extensions to the mapping here
    };

    vector<map<string, string>> data = listDir(folderPath, extToIcon);

    stringstream jsonResult;
    jsonResult << "{ \"data\": [";
    for (size_t i = 0; i < data.size(); ++i) {
        jsonResult << "{";
        for (const auto& [key, value] : data[i]) {
            jsonResult << "\"" << key << "\": \"" << value << "\", ";
        }
        jsonResult.seekp(-2, jsonResult.cur); // Remove trailing comma and space
        jsonResult << "}";
        if (i < data.size() - 1) {
            jsonResult << ",";
        }
    }
    jsonResult << "] }";

    return jsonResult.str();
}

int main() {
    string jsonData = loadFs();
    if (!jsonData.empty()) {
        cout << jsonData << endl;
    }
    return 0;
}
