#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define STR_LEN 100
#define MAX_ARGS 100
#define tablesize 1000
extern int errno;

struct node {
    char* key;
    char* val;
    struct node* next;
};
struct node* t[tablesize];

struct node* new_node(char *k, char* v) {
    // @param: char* k: key of the node
    //         char* v: value of node
    // return: node* initialized with k and v set as key and value
    struct node* n = (struct node*)(malloc(sizeof(struct node)));
    n->key = strdup(k);
    n->val = strdup(v);
    n->next = NULL;
    return n;
}
void init_table() {
    for(int i=0; i<tablesize; i++) {
        t[i] = NULL;
    }
}

unsigned int hash(char* s) {
    unsigned int hash = 5381;
    int c;
    while(c = *s++) {
        hash = ((hash<<5) + hash) + c;
    }
    return hash;
}
int find(char* s) {
    // @param: char* k: key to be looked up in the hashtable
    // return: 1 if found, -1 if not found
    int idx = hash(s) % tablesize;
    if(t[idx] == NULL) {
        printf("key does not exist\n");
        return -1;
    }
    int i;
    for(i=0; i<tablesize;i++) {
        struct node* curr = t[i];
        while(curr) {
            if(strcmp(curr->key, s)==0) {
                return 1;
            }
            curr = curr->next;
        }
    }
    return -1;
}
void insert(char* k, char* v) {
    // check if key already exits in the hashtable t
    //if(find(k,t) == 1)
    //    return;
    // create a new node
    struct node* ptr;
    int found = 0;
    int idx = hash(k) % tablesize;
    ptr = t[idx];
    while(ptr != NULL) {
        if(strcmp(ptr->key, k) == 0 ) {
            found = 1;
            break;
        }
        ptr = ptr->next;
    }
    if(found) {
        printf("Alias named \"%s\" already exists!\n", k);
    }
    else {
        struct node* n = new_node(k,v);
        n->next = t[idx];
        t[idx] = n;
    }
}
void print_node(struct node* c) {
    if(c == NULL) {
        printf("Node c does not exist!\n");
        exit(1);
    }
    printf("alias %s=\'%s\'\n", c->key, c->val);
}
void print_table() {
    if(!t) {
        printf("table does not exist!\n");
        exit(1);
    }
    for(int i=0;i<tablesize; i++) {
        struct node* curr = t[i];
        while(curr != NULL) {
            print_node(curr);
            curr = curr->next;
        }
    }
}
void remove_key(char* s) {
    int idx = hash(s) % tablesize;
    if(!t[idx]) {
        printf("key does not exist");
    }
    struct node* ptr = t[idx];
    struct node* prev = NULL;
    struct node* temp;
    // check if the head of the linked list is the key to be removed
    if(strcmp(t[idx]->key, s) == 0) {
        // check if there are other nodes in the list
        printf("key found, removing key now\n");
        temp = t[idx];
        if(t[idx]->next) {
            t[idx]->next = t[idx]->next->next;
        }
        else
            t[idx] = NULL;
        free(temp);
        return;
    }
    while(ptr) {
        if(strcmp(ptr->key, s) == 0) {
            printf("key found, removing key now\n");
            struct node* next = ptr->next;
            temp = ptr;
            if(prev)
                prev->next = next;
//            else { //if the head of the linked list is the key to be removed
//                if(t[idx]->next) {
//                    t[idx]->next = t[idx]->next->next;
//                }
//                else
//                    t[idx] = NULL;
//            }
            free(temp);
            break;
        }
        prev = ptr;
        ptr = ptr->next;
    }
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
void handle_alias(char** args) {
    if(args[1] == NULL) {
        printf("Usage:\n");
        printf("alias -p   \"display the list of currently saved aliases\"\n");
        printf("alias <alias_name>=<command>    \"To store a new alias\"\n");
        return;
    }
    if(strcmp(args[1], "-p")== 0) {
        print_table();
        return;
    }

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
    // add the alias command
    if(strcmp(args[0], "alias") == 0) {
        handle_alias(args);
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

    //init_table();
    //char* k1 = "hello";
    //char* v1 = "world";
    //char* k2 = "this";
    //char* v2 = "test";
    //char* k3 = "doesIt";
    //char* v3 = "work";
    //insert(k1,v1);
    //  insert(k2,v2);
    //  insert(k3,v3);
    //print_table();
    //printf((find(k2))==1?"Key \'%s\' found\n":"key \'%s\' not found\n", k2);
    //remove_key(k1);
    //printf((find(k1))==1?"Key \'%s\' found\n":"key \'%s\' not found\n", k1);
    //  insert(k1,v3);
    //print_table();
    return 0;
}
