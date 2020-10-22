#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STR_LEN 100


int run_cmds(char* buf) {
    char s[STR_LEN];
    char** res = NULL;
    int num =0;
    int idx = 0;
    char* ext = "exit";
    char* ech = "echo";
    char* cmd;
    int i;
    int t;
    pid_t p;
    strcpy(s, buf);
    char* c = strtok(s, " \n");
    num++;

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
    res = realloc(res, sizeof(char*) * num);
    res[idx] = NULL;
    cmd = res[0];
    // check the first words of the command line
    // if the command is exit
    if(strcmp(cmd,ext)==0) {
        printf("Exiting!\n");
        exit(1);
    }
    // if the command is echo
    if(strcmp(cmd,ech)==0) {
        i=1;
        while(res[i] != NULL) {
            printf("%s ",res[i]);
            i += 1;
        }
        printf("\n");
    }
    // if the command is something else
    else {
        p = fork();
        if(p < 0) {
            fprintf(stderr, "Unable to fork\n");
            exit(1);
        }
        if(p == 0) {
            // child process running
            execvp(cmd, res);
            fprintf(stderr, "Unable to run command\n");
        }
        wait(&t);
    }
    free(res);
    return 0;
}

void user_loop() {
    char *buf = malloc(sizeof(char[STR_LEN]));
    int num = 0;
    int idx = 0;
    int r;
    while(1)  {
        printf(">");
        fgets(buf, STR_LEN, stdin);
        if(buf== NULL) {
            fprintf(stderr, "Unable to get command line \n");
            exit(1);
        }
        r = run_cmds(buf);
        if(r == -1)
            exit(1);
    }
}

int main(int argc, char* argv[]) {
    user_loop();
}
