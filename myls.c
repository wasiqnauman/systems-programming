#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>




struct node {
    struct dirent* dir_entry;
    struct stat* info;
};

char* get_type(char d_type) {
    if(d_type == DT_DIR)
        return "D";
    if(d_type == DT_REG)
        return "F";
}
void print_info(struct stat* info, char* s) {
    if(strchr(s,'s') != NULL)
        printf("size: %d\n", info->st_size);
    if(strchr(s,'o'))
        printf("owner: %d\n", info->st_uid);
    if(strchr(s,'i'))
        printf("inode: %d\n", info->st_ino);
}

void run_ls(int argc, char* argv[]) {
    struct stat* buf = malloc(sizeof(struct stat));
    char* name = malloc(sizeof(char[100]));
    char* args = malloc(sizeof(char[100]));
    if(argc >= 2) {
        if(strchr(argv[1], '-'))
            args = strtok(argv[1], "-");
        else {
            strcpy(name, argv[1]);
            args = strtok(argv[2], "-");
            printf("%s\n", args);
            
        }
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
        stat(entry->d_name, buf);
        char type[100];
        //char* type = get_type(entry->d_type);
        if(S_ISDIR(buf->st_mode))
            strcpy(type, "D");
        if(S_ISREG(buf->st_mode))
            strcpy(type, "F");
        printf("%s: %s\n",type, entry->d_name);
        print_info(buf, args);
    }
    perror("readdir");
    closedir(dir);

}
void use_scandir(int argc, char* argv[]) {
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
    //use_scandir(argc,argv);
    run_ls(argc, argv);
}


