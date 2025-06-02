#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void print_info(const char *path, const char *name) {
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    // struct stat save file infomation
    struct stat file_info;
    if (stat(fullpath, &file_info) != 0) {
        perror("stat");
        return;
    }

    // owner
    struct passwd *pw = getpwuid(file_info.st_uid);
    const char *owner = pw ? pw->pw_name : "unknown";

    // type
    // macro S_ISDIR check if it is dir or not
    char *type = S_ISDIR(file_info.st_mode) ? "Folder" : "File";

    // time
    char create_time[64], modify_time[64];

    // birth time is not standardized, but st_ctime is often used as creation on ext4
    strftime(create_time, sizeof(create_time), "%Y-%m-%d %H:%M:%S", localtime(&st.st_ctime));

    strftime(modify_time, sizeof(modify_time), "%Y-%m-%d %H:%M:%S", localtime(&st.st_mtime));

    printf("%-30s %-10s %-15s Created: %-20s Modified: %-20s\n",
           name, type, owner, create_time, modify_time);
}

int main(int argc, char *argv[]) {
    const char *dir = (argc > 1) ? argv[1] : ".";

    DIR *d = opendir(dir);
    if (!d) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    go through all entry of current dir
    while ((entry = readdir(d)) != NULL) {
        // not handle ./ and ../ directory
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            print_info(dir, entry->d_name);
    }

    closedir(d);
    return 0;
}
