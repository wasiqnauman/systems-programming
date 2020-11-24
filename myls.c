#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

int main() {
    DIR* dir = opendir(".");
    if(!dir) {
        fprintf(stderr, "Dir is NULL\n");
        exit(1);
    }
    struct dirent* entry;
    while((entry = readdir(dir) != NULL)){
        printf("%s\n", entry->d_name);
    }
    perror("readdir");
    closedir(dir);
    return 0;
}
