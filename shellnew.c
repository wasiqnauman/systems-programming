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
    // reads line from standard input
    // @param: none
    // returns: string entered by user

    char* buf = malloc(sizeof(char[STR_LEN]));
    fgets(buf, STR_LEN, stdin);
    if(buf == NULL) {
        perror("Unable to read command\n");
        exit(1);
    }
    return buf;
}
char** parse_commands(char *line) {
    // split the string entered by user into a array of commands seperated by space
    // @param: string entered by user
    // return: array of commands
    //
    int num = 0;  // number of elements in the array
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
        argv = realloc(argv, sizeof(char*) * num);   // allocate new memory for each new token added
        if(argv == NULL) {
            fprintf(stderr, "Unable to realloc\n");
            exit(1);
        }
        // insert token into the argv array
        argv[idx++] = token;
        token = strtok(NULL, " \n");
        num++;
    }
    // set the last element of the array as NULL (required for execvp())
    argv = realloc(argv, sizeof(char*) * num);
    argv[idx] = NULL;

    return argv;
}

int check_exit(char** args) {
    // check if the user has entered "exit" into the command line
    // @param: array of arguments
    // return: 1 if user wants to exit
    //         0 otherwise

    if(strcmp(args[0], "exit") == 0)
        return 1;
    else
        return 0;
}
void run_commands(char** args) {
    // runs the commands entered by the user into the shell
    // @param: array of arguments
    // return: nothing

    pid_t p;
    int status;

    // check if the command was to echo
    if(strcmp(args[0], "echo")==0) {
        print_array(args, 1);
        return;
    }
    
    // run the commands on a new process
    p = fork();
    if(p < 0) {
        perror("fork\n");
        exit(1);
    }
    if(p == 0) {
        //child process
        // run the commands in the child process
        execvp(args[0],args);
        fprintf(stderr, "Unable to run command\n");
    }
    wait(&status);   // wait for the child process to exit
}
void user_loop() {
    // get the current working directory of the user
    char cwd[STR_LEN];
    if(getcwd(cwd, sizeof(cwd))==NULL) {
        perror("CWD\n");
    }
    while(1) {
        printf("%s>", cwd);
        char* line = read_line();
        char** args = parse_commands(line);

        // check if the command was empty
        if(args[0] == NULL) continue;

        // check if the user has signaled to exit
        if(check_exit(args) == 1)
            break;
        run_commands(args);  
    }
}
int main(int argc, char* argv[]) {
    user_loop();
}
