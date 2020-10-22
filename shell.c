#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STR_LEN 100


char* make_arr(char* buf) {
    char s[STR_LEN];
    char** res = NULL;
    int num =0;
    int idx = 0;
    strcpy(s, buf);
    char* c = strtok(s, " ");
    num++;

    while(c != NULL) {
        if(strcmp(c, "\n")==0) {
            printf("New Line character detected!\n");
            c = strtok(NULL, " ");
            printf("New char is %s", c);
        }
        res = realloc(res, sizeof(char*) * num);
        if(res == NULL) {
            fprintf(stderr, "Failed to allocate memory for array \n");
            exit(1);
        }   
        res[idx++] = c;
        c = strtok(NULL, " ");
        num++;
    }
    res = realloc(res, sizeof(char*) * num);
    res[idx] = NULL;

    for(int i=0; i< num; i++) {
        printf("\"%s\"\n", res[i]);
    }
}
void user_loop() {
    char *buf = malloc(sizeof(char[STR_LEN]));
    int num = 0;
    int idx = 0;
    char* arr;
    while(1)  {
        printf(">");
        fgets(buf, STR_LEN, stdin);
        if(buf== NULL) {
            fprintf(stderr, "Unable to get command line \n");
            exit(1);
        }
        arr = make_arr(buf);
        

    }
}

int main(int argc, char* argv[]) {
    user_loop();
}
