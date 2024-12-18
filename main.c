#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Utility function to check if a path is a directory
int isDirectory(const char *path) {
    struct stat info;
    if (stat(path, &info) != 0) {
        return 0; // Cannot access path
    }
    return (info.st_mode & S_IFDIR) != 0;
}

// Function to determine file extension and set icon based on a mapping
const char* getFileIcon(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (ext && ext != filename) {
        ext++; // Move past the dot
        if (strcmp(ext, "py") == 0) return "fab fa-python";
        if (strcmp(ext, "java") == 0) return "fab fa-java";
        if (strcmp(ext, "html") == 0) return "fab fa-html5";
        if (strcmp(ext, "css") == 0) return "fab fa-css3";
        if (strcmp(ext, "js") == 0) return "fab fa-js";
        if (strcmp(ext, "c") == 0) return "fab fa-cuttlefish";
    }
    return "fa fa-file";
}

// Recursive function to list directory contents
void listDir(const char *path, int maxDepth, FILE *output) {
    if (maxDepth <= 0) {
        return;
    }

    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Error: Could not open directory %s\n", path);
        return;
    }

    fprintf(output, "[\n");
    int first = 1;

    while ((entry = readdir(dir)) != NULL) {
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (first) {
            first = 0;
        } else {
            fprintf(output, ",\n");
        }

        if (isDirectory(fullPath)) {
            fprintf(output, "  {\n    \"text\": \"%s\",\n    \"nativeURL\": \"%s\",\n    \"type\": \"directory\",\n    \"children\": [\n", entry->d_name, fullPath);
            listDir(fullPath, maxDepth - 1, output);
            fprintf(output, "    ]\n  }\n");
        } else {
            fprintf(output, "  {\n    \"text\": \"%s\",\n    \"nativeURL\": \"%s\",\n    \"type\": \"file\",\n    \"icon\": \"%s\"\n  }\n", entry->d_name, fullPath, getFileIcon(entry->d_name));
        }
    }

    fprintf(output, "]");
    closedir(dir);
}

// Function to load file system data
void loadFs() {
    FILE *folderFile = fopen("/storage/emulated/0/.Apps/Ace-Code/folder.txt", "r");
    if (!folderFile) {
        fprintf(stderr, "Error: Could not open folder.txt\n");
        return;
    }

    char folderPath[1024];
    if (!fgets(folderPath, sizeof(folderPath), folderFile)) {
        fclose(folderFile);
        fprintf(stderr, "Error: Could not read folder path\n");
        return;
    }
    fclose(folderFile);

    // Remove trailing newline
    size_t len = strlen(folderPath);
    if (len > 0 && (folderPath[len - 1] == '\n' || folderPath[len - 1] == '\r')) {
        folderPath[len - 1] = '\0';
    }

    // Output JSON to stdout
    printf("{ \"data\": ");
    listDir(folderPath, 10, stdout);
    printf(" }\n");
}

int main() {
    loadFs();
    return 0;
}
