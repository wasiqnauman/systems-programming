#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
int main(int argc, char* argv[]) {
    char* name = malloc(sizeof(char[100]));
    if(argc >= 2) {
        strcpy(name, argv[1]);
    }
    else
        strcpy(name, ".");
    DIR* dir = opendir(name);
    if(!dir) {
        fprintf(stderr, "Dir is NULL\n");
        exit(1);
    }
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL){
        printf("%s\n", entry->d_name);
    }
    perror("readdir");
    closedir(dir);
    return 0;
}
