#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>


char* get_type(char d_type) {
    if(d_type == DT_DIR)
        return "D";
    if(d_type == DT_REG)
        return "F";
}
void run_ls(int argc, char* argv[]) {
    struct stat buf;
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
        stat(entry->d_name, &buf);
        char type[100];
        //char* type = get_type(entry->d_type);
        if(S_ISDIR(buf.st_mode))
            strcpy(type, "D");
        if(S_ISREG(buf.st_mode))
            strcpy(type, "F");
        printf("%s: %s\n",type, entry->d_name);
    }
    perror("readdir");
    closedir(dir);

}
void scanFunc(int argc, char* argv[]) {
struct dirent** names;
    int n;
    if(argc < 2)
        argv[1] = ".";
    n = scandir(argv[1], &names, NULL, alphasort);
    if(n < 0)
        perror("scandir\n");
    else
    {
        for(int i=0; i<n; i++) {
            struct stat* buf;
            int r = stat(names[i]->d_name, buf);
            if(r< 0) {
                perror("stat\n");
                exit(1);
            }
            char type[100];
            if(S_ISREG(buf->st_mode))
                strcpy(type, "F");
            if(S_ISDIR(buf->st_mode))
                strcpy(type, "D");
            //char* type = get_type(names[i]->d_type);
            printf("%s: %s\n", type, names[i]->d_name);
            free(names[i]);
        }
    }

}
int main(int argc, char* argv[]){
    run_ls(argc, argv);
}

