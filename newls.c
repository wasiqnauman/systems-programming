#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>

#define STR_LEN 100
#define MAX_ARGS 100

extern int errno;

void print_permissions(struct stat* info) {

    printf("permissions: \t");
    printf( (info->st_mode & S_IRUSR) ? "r" : "-");
    printf( (info->st_mode & S_IWUSR) ? "w" : "-");
    printf( (info->st_mode & S_IXUSR) ? "x" : "-");
    printf( (info->st_mode & S_IRGRP) ? "r" : "-");
    printf( (info->st_mode & S_IWGRP) ? "w" : "-");
    printf( (info->st_mode & S_IXGRP) ? "x" : "-");
    printf( (info->st_mode & S_IROTH) ? "r" : "-");
    printf( (info->st_mode & S_IWOTH) ? "w" : "-");
    printf( (info->st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");
}
void print_info(char* name, char* args) {
    // -s size
    // -o owner
    // -i I-node number
    // -p protections(mode of file);
    // -m time & date of modication
    // -c time & date of creation
    struct stat* info = malloc(sizeof(struct stat));
    stat(name, info);
    printf("name: \t%s\n", name);
//    if(S_ISDIR(info->st_mode))
//        printf("type: \tD\n");
    if(S_ISREG(info->st_mode))
        printf("type: \tF\n");
    else
        printf("type: \tD\n");
    if(!args) {
        return;
    }
    if(strchr(args, 's'))
        printf("size: \t%d\n", info->st_size);
    if(strchr(args, 'o')) {
        struct passwd* pw = getpwuid(info->st_uid);
        if(pw != 0)
            printf("owner: \t%s\n", pw->pw_name);
    }
    if(strchr(args,'i'))
        printf("inode: \t%d\n", info->st_ino);
    if(strchr(args, 'p')) {
        print_permissions(info);
    }
    if(strchr(args, 'm')) {
        struct tm* tm;
        tm = localtime(&info->st_mtimespec.tv_sec);
        printf("modified: \t%d-%d-%d %d:%d:%d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    if(strchr(args, 'c')) {
        struct tm* tm;
        tm = localtime(&info->st_ctimespec.tv_sec);
        printf("created: \t%d-%d-%d %d:%d:%d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
}
int comp(const void* a, const void* b) {
    const char* pa = *(const char**)a;
    const char* pb = *(const char**)b;
    // printf("Comparing %s vs. %s for result %d\n", pa, pb, strcmp(pa,pb));

    return strcmp(pa,pb);
}
void read_info(DIR* dir, char* args) {
    struct stat info;
    struct dirent* entry;
    char** dir_list;
    int size = 0;
    while((entry = readdir(dir)) != NULL) {
        size++;
        dir_list = realloc(dir_list, sizeof(char*) * size);
        stat(entry->d_name, &info);
        dir_list[size-1] = strdup(entry->d_name);
        //print_info(info, entry, args);
    }
    qsort(dir_list, size, sizeof(char*), comp);
    printf("Files in dir: %d\n", size);
    for(int i=0; i<size; i++) {
        print_info(dir_list[i], args);
    }
    closedir(dir);
}
void myls(int argc, char* argv[]) {
    char** names = NULL;
    char* args = NULL;
    DIR* dir;
    int size = 0;
    int start = 1;
    if(argc > 1) {
        // check if any arguments were given
        if(strchr(argv[1], '-')) {
            args = strdup(strtok(argv[1], "-"));
            start = 2;
        }
        // pick up the filenames
        // if options were given then start picking up from argv[2] otherwise start from argv[1]
        // if only options were given then the loop will not run
        for(int i=start; i<argc; i++) {
            if(argv[i] == NULL)
                break;
            size++;
            names = realloc(names, sizeof(char[STR_LEN])*size);
            names[size-1] = strdup(argv[i]);
        }

    }

//    printf("arguments: %s\n", args);
//    printf("Num files: %d\n", size);
//    if(!names)
//        printf("Not files specified\n");
//    else
//        for(int i=0; i<size; i++) {
//            printf("%s\n", names[i]);
//        }

    // if no filenames given then list all files in current directory
    if(!names) {
        dir = opendir(".");
        read_info(dir, args);
    }
    else {
        // list info of all the filenames given
        for(int i=0; i<size; i++) {
            dir = opendir(names[i]);
            if(!dir) {
                // check if the given names were of files and not directories
                struct stat info;
                if((stat(names[i], &info) == -1)) {
                    perror("State\n");
                    printf("File \"%s\" not found\n", names[i]);
                    continue;    
                }
                else
                    print_info(names[i], args);
            }
            else
                read_info(dir, args);
            //dir was closed in read_info
        }
    }
}
int main(int argc, char* argv[]) {
    myls(argc, argv);
    return 0;
}

