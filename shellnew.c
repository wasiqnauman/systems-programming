#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define STR_LEN 100
#define MAX_ARGS 100

extern int errno;


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

char* read_line() {
    char* buf = malloc(sizeof(char[STR_LEN]));
    fgets(buf, STR_LEN, stdin);
    if(buf == NULL) {
        perror("Unable to read command\n");
        exit(1);
    }
    return buf;
}
char** parse_commands(char *line) {
    int num = 0;
    int idx = 0;
    char** argv = malloc(num*sizeof(char*));
    char* token;

    if(argv == NULL) {
        fprintf(stderr, "Malloc error\n");
        exit(1);
    }
    token = strtok(line, " \n");
    num++;
    
    while(token != NULL) {
        argv = realloc(argv, sizeof(char*) * num);
        if(argv == NULL) {
            fprintf(stderr, "Unable to realloc\n");
            exit(1);
        }
        argv[idx++] = token;
        token = strtok(NULL, " \n");
        num++;
    }
    argv = realloc(argv, sizeof(char*) * num);
    argv[idx] = NULL;
    return argv;
}
int check_exit(char** args) {
    if(strcmp(args[0], "exit") == 0)
        return 1;
    else
        return 0;
}
void run_commands(char** args) {
    pid_t p;
    int status;

    // check if the command was to echo
    if(strcmp(args[0], "echo")==0) {
        print_array(args, 1);
        return;
    }
    p = fork();
    if(p < 0) {
        perror("fork\n");
        exit(1);
    }
    if(p == 0) {
        //child process
        execvp(args[0],args);
        fprintf(stderr, "Unable to run command\n");
    }
    wait(&status);
}
void user_loop() {
    char cwd[STR_LEN];
    if(getcwd(cwd, sizeof(cwd))==NULL) {
        perror("CWD\n");
    }
    while(1) {
        printf("%s>", cwd);
        char* line = read_line();
        char** args = parse_commands(line);
        // check if the user has signaled to exit
        if(check_exit(args) == 1)
            break;
        run_commands(args);  
    }
}
int main(int argc, char* argv[]) {
    user_loop();
}
