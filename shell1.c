#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define STR_LEN 100
#define MAX_ARGS 100

extern int errno;

void parsecmds(char* str, char* argv[]) {
    int idx = 0;
    char *c = strtok(str, " \n");
    while(c != NULL) {
        argv[idx++] = c;
        c = strtok(NULL, " \n");
    }
    argv[idx] = NULL;
}
char** toarray(char* buf) {
    char s[STR_LEN];
    char** res = NULL;
    int num =0;
    int idx = 0;
    strcpy(s, buf);
    char* c = strtok(s, " \n");
    num++;

    // create the array of arguments
    while(c != NULL) {
        res = realloc(res, sizeof(char*) * num);
        if(res == NULL) {
            fprintf(stderr, "Failed to allocate memory for array \n");
            exit(1);
        }
        res[idx++] = c;
        c = strtok(NULL, " \n");
        num++;
    }
    res = realloc(res, sizeof(char*) * num);   // add NULL as the last argument
    res[idx] = NULL;
    return res;   //return the array of arguments
}
void print_array(char ** args, int i) {
    // @param: char** args - array to be printed
    //         int i - index to start printing from
    // print the array from the ith index till the end

    for(i; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    //print newline after printing the array
    printf("\n");
}
void run_commands(char* argv[]) {
    pid_t p;
    int status;
    p = fork();
    if (p<0) {
        perror("Unable to fork\n");
        exit(1);
    }
    if(p==0) {
        int r = execvp(argv[0], argv);
        if(r < 0) {
            perror("exec\n");
            exit(1);
        }
    }
    else
        wait(&status);
}
void user_loop() {
    char *buf = malloc(sizeof(char[STR_LEN]));
    int num = 0;
    int idx = 0;
    pid_t p;
    int s;
    int r;
    char* args[MAX_ARGS];
    while(1)  {
        printf(">");
        fgets(buf, STR_LEN, stdin);
        if(buf== NULL) {
            fprintf(stderr, "Unable to get command line \n");
            exit(1);
        }
        parsecmds(buf,args);
//        args = toarray(buf);
        if(args[0] == NULL) continue; // no command provided
        if(strcmp(args[0], "exit") == 0) {
            printf("Exiting!\n");
            exit(0);
        }
        else if(strcmp(args[0], "echo") == 0) {
            print_array(args, 1);   // print the rest of the command starting from 1st index since 0th is the command itself
            continue;
        }
        else {
            print_array(args, 0);
            run_commands(args);
        }
    }
}

int main(int argc, char* argv[]) {
    user_loop();
}
