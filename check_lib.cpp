#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grp.h>
#include <unistd.h>

int main() {
    gid_t gid = getgid(); // Get the current group ID
    struct group grp;     // Declare a group structure
    struct group *result; // Pointer to hold the result
    char buffer[1024];    // Buffer for the group name
    int s;                // Status variable

    s = getgrgid_r(gid, &grp, buffer, sizeof(buffer), &result);
    if (s != 0) {
        perror("getgrgid_r failed"); // Print error if it fails
        return 1; // Exit with error code
    }

    printf("Group ID: %d\n", gid);
    printf("Group Name: %s\n", grp.gr_name); // Print the group name
    return 0; // Exit successfully
}
