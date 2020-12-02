#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define STR_LEN 1000
#define MAX_ARGS 100
#define tablesize 10000
extern int errno;

struct node {
    char* key;
    char* val;
    struct node* next;
};

// hashtable for aliases
struct node* t[tablesize];
// hashtable for shell variables
struct node* shellv[tablesize];

// functionality for aliases
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
void __init__() {
    // initialize the hashtables for alias and shell variables
    for(int i=0; i<tablesize; i++) {
        t[i] = NULL;
        shellv[i] = NULL;
    }
}

unsigned int hash(char* s) {
    unsigned int hash = 5381;
    int c;
    while((c = *s++)) {
        hash = ((hash<<5) + hash) + c;
    }
    return hash;
}
int find(char* s) {
    // @param: char* k: key to be looked up in the hashtable
    // return: 1 if found, -1 if not found
    int idx = hash(s) % tablesize;
    if(t[idx] == NULL) {
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
        printf("alias \"%s\" mapped to \"%s\" added\n", k, v);
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
    if(t==NULL) {
        printf("table does not exist!\n");
        exit(1);
    }
    int num = 1;
    for(int i=0;i<tablesize; i++) {
        struct node* curr = t[i];
        while(curr != NULL) {
            printf("%d. ", num++);
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
struct node* get_node(char* key) {
    // return NULL if key does not exist
    if(find(key) == -1) {
        return NULL;
    }
    int idx = hash(key) % tablesize;
    struct node* ptr = t[idx];
    struct node* item = NULL;
    while(ptr != NULL) {
        if(strcmp(ptr->key, key) == 0) {
            item = ptr;
            break;
        }
        ptr = ptr->next;
    }
    return item;
}

void print_array(char ** args, int start) {
    // @param: char** args - array to be printed
    //         int i - index to start printing from
    // print the array from the ith index till the end
    
    for(int i=start; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    //print newline after printing the array
    printf("\n");

}
// functionality for shell variables

int find_var(char* key) {
    int idx = hash(key) % tablesize;
    struct node* ptr = shellv[idx];
    int found = 0;
    while(ptr != NULL) {
        if(strcmp(ptr->key, key)== 0) {
            found = 1;
            break;
        }
        ptr = ptr->next;
    }
    if(found)
        return 1;
    else
        return 0;
}
void insert_var(char* key, char* val) {
    if(find_var(key)) {
        printf("Var \"%s\" already exists!\n", key);
        return;
    }
    int idx = hash(key) % tablesize;
    struct node* n = (struct node*)malloc(sizeof(struct node));
    n->key = strdup(key);
    n->val = strdup(val);
    n->next = shellv[idx];
    shellv[idx] = n;
    printf("Var \"%s\" added successfully\n", key);
}
void print_var(struct node* n) {
    printf("%s=%s\n", n->key, n->val);
}
void print_vars() {
    int num = 1;
    for(int i=0; i<tablesize; i++) {
        struct node* ptr = shellv[i];
        while(ptr != NULL) {
            printf("%d: ", num++);
            print_var(ptr);
            ptr = ptr->next;
        }
    }
}
struct node* get_var(char* k) {
    int idx = hash(k) % tablesize;
    struct node* ptr = shellv[idx];
    int found = 0;
    while(ptr != NULL) {
        if(strcmp(ptr->key, k) == 0) {
            found = 1;
            break;
        }
        ptr = ptr->next;
    }
    if(found)
        return ptr;
    return NULL;
}
char* edit(char* line) {
    // @param: line containing the shell variable
    // return: line with the shell variable replaced
    // This functions looks up the shell variable starting with $ and replaces it with its value in the hashtable

    int num = 0;  // number of elements in the array
    int idx = 0;
    char** words = malloc(num*sizeof(char*));
    char* token;
    char* buf = strdup(line);
    //if memory allocation failed
    if(words == NULL) {
        fprintf(stderr, "Malloc error\n");
        exit(1);
    }
    
    //split the input line into an array of strings
    token = strtok(buf, " \n");
    num++;

    while(token != NULL) {
        words = realloc(words, sizeof(char*) * num);   // allocate new memory for each new token added
        if(words == NULL) {
            fprintf(stderr, "Unable to realloc\n");
            exit(1);
        }
        // insert token into the words array
        words[idx++] = token;
        token = strtok(NULL, " \n");
        num++;
    }

    //replace the $shellv with its value in the shellv[] hashtable (if any)
    int total_length = 0;
    char* var;
    for(int i=0; i<idx; i++) {

        // check if the current word is the $shell_variable
        if(strchr(words[i], '$')) {

            // check if the $shell_variable has been set
            // var will contain the word starting from $ symbol
            // we will increment var so we get the word after the $ symbol
            var = strchr(words[i], '$');
            var++;
            printf("Variable:%s\n", var);
            if(find_var(var) == 0) {
                fprintf(stderr, "Shell variable being used is not set\n");
                return NULL;
            }

            // get the value of $shell_variable
            //struct node* n = get_var(words[i]);
            
            struct node*n = get_var(var);
//            int index = hash(words[i]) %tablesize;
//            words[i] = strdup(shellv[index]->val);

//          // replace the $shell_variable with its set value
            words[i] = strdup(n->val);        
        }

        total_length += strlen(words[i]);
//        printf("%d:%s ",i, words[i]);
    }
    total_length++;
    //for(int i=0; i<idx; i++) {
    //    printf("%d:%s\n",i, words[i]);
    //}
    //printf("\n");

    // combine the array of words into a single string
    // allocate memory for the joined string
    char* res = malloc(sizeof(char[STR_LEN]));
    // mark string as empty, so we can copy to it
    res[0] = '\0';
    int len;
    for(int i=0; i<idx; i++) {
       // printf("adding %s to string\n", words[i]);
        strcat(res, words[i]);
        // add space after each word unless its the last word;
        if(words[i+1] != NULL)
            strcat(res," ");
        if(words[i+1] == NULL)
            strcat(res, "\0");
    }

    return res;
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
    //since strtok will modify the original string which we dont want, we create a buffer and pass it to strtok
    char* buf = strdup(line);
    int num = 0;  // number of elements in the array
    int idx = 0;
    char** argv = malloc(num*sizeof(char*));
    char* token;

    if(argv == NULL) {
        fprintf(stderr, "Malloc error\n");
        exit(1);
    }

    token = strtok(buf, " \n");
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
char** parse_alias(char *line) {
    // DIFFERENT THAN parse_commands: parse_commands has a different delimiter
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

    token = strtok(line, "=\n");
    num++;
    
    while(token != NULL) {
        argv = realloc(argv, sizeof(char*) * num);   // allocate new memory for each new token added
        if(argv == NULL) {
            fprintf(stderr, "Unable to realloc\n");
            exit(1);
        }
        // insert token into the argv array
        argv[idx++] = token;
        token = strtok(NULL, "=\n");
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
char* arr_to_str(char** args) {
    char* cmd = malloc(sizeof(char[STR_LEN]));
    int i = 1;
    while(args[i] != NULL) {
        strcat(cmd, args[i]);
        if(args[i+1] != NULL)
            strcat(cmd, " ");
        i++;
    }
    return cmd;
}
void handle_alias(char** args) {
    // if an alias command has been entered, this function handles it
    if(args[1] == NULL) {
        printf("Usage:\n");
        printf("alias \t-p \t\t\t\"display the list of currently saved aliases\"\n");
        printf("alias \t<alias_name>=<command>  \"To store a new alias\"\n");
        return;
    }
    // if alias -p then display all previosly stored aliases
    if(strcmp(args[1], "-p")== 0) {
        printf("List of currently saved aliases:\n");
        print_table();
        return;
    }
    //args[1] will contain the alias command, we need to split it before and after the '=' sign
    char* command = arr_to_str(args);
    char* key = strdup(strtok(command,"=\n"));
    char* value = strdup(strtok(NULL, "=\n"));
   // printf("value: %s\n", value);
    insert(key, value);
    free(key);
    free(value);
    free(command);
}

void run_alias(char** args) {
    // if an alias has been used, this function runs it
    char* key = strdup(args[0]);
    // get the node containing the alias
    struct node* c = get_node(key);
    char* val = strdup(c->val);
    printf("val: %s\n", val);
    char** cmd = parse_commands(val);
    print_array(cmd, 0);
    pid_t p;
    int s;
    p = fork();
    if(p<0) {
        perror("fork\n");
        exit(1);
    }
    if(p==0) {
        printf("cmd:%s\n", cmd[0]);
        execvp(cmd[0], cmd);
        fprintf(stderr, "Unable to run alias: %s\n", strerror(errno));
    }
    wait(&s);
}

void add_shellv(char** args) {
    // this function adds a shell variable to the hashtable


    if(args[1] == NULL) {
        printf("Usage:\n");
        printf("set \t-l \t\t\t\"display the list of currently saved shell variables\"\n");
        printf("set \t<var_name>=<val> \t\"To store a new shell variable\"\n");
        return;
    }
    // if alias -l then list all previosly stored shell variables
    if(strcmp(args[1], "-l")== 0) {
        printf("List of currently set shell variables:\n");
        print_vars();
        return;
    }
    //args[1] will contain the variable assignment, we need to split it before and after the '=' sign
    char* command = arr_to_str(args);
    char* key = strdup(strtok(command,"=\n"));
    char* value = strdup(strtok(NULL, "=\n"));
    printf("key:%s\nvalue:%s\n",key, value);
    if(strchr(key, '$')) {
        printf("Cannot use $ while setting shell variable\n");
        return;
    }
    insert_var(key, value);
    free(key);
    free(value);
    free(command);


}
void run_commands(char** args) {
    // runs the commands entered by the user into the shell
    // @param: array of arguments
    // return: nothing

    pid_t p;
    int status;

    //args[0] contains the commands that are entered into the shell

    //check if a shell variable is being set
    if(strcmp(args[0], "set") == 0) {
       // printf("set command detected\n");
        add_shellv(args);
        return;
    }
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
    // check if an alias was entered
    if(find(args[0]) == 1) {
        run_alias(args);
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
int check_shellv(char* line) {
    //printf("line:%s\n", line);
    char** commands = parse_commands(line);
    int i=0;
    int set = 0;
   
    // if a shell variable is being set, dont look for it yet
   while(commands[i] != NULL) {
       if(strcmp(commands[i], "set")==0) {
           // set command is being to set a shell variable
           set = 1;
           break;
       }
       i++;
   }
   // return true only if a shell variable is being used without the set command
   if(set == 0 && strchr(line,'$')) {
        // printf("shellv being used without set command\n");
       return 1;
   }
   else if(set == 1 && strchr(line, '$')) {
       // printf("shellv being used with the set command\n");
       // this will be handled in add_shellv();
       return 0;
   }
   else {
      //  printf("shellv not being used at all\n");
       return 0;
   }
}
void user_loop() {
    // get the current working directory of the user
    char cwd[STR_LEN];
    if(getcwd(cwd, sizeof(cwd))==NULL) {
        perror("CWD\n");
    }

    while(1) {
        printf("%s>", cwd);
        char** args;
        char* line = read_line();
        //check if any shell variables are being used
        if(check_shellv(line))
        {
           // printf("shellv found, handling it...\n");
            // replace shell variables with their values and parse the commands
            char* new_line = edit(line);

            // edit(line) would return null if a $ was used while setting shell variable
            if(new_line == NULL)
                continue;
            args = parse_commands(new_line);
        }
        // otherwise use the normal line if no shell variables
        else
            args = parse_commands(line);

        // check if the command was empty
        if(args[0] == NULL) continue;

        // check if the user has signaled to exit
        if(check_exit(args) == 1)
            break;

        run_commands(args);  
    }
}
int main() {
    __init__();
    user_loop();
   // for(int i=0; i<tablesize; i++) {
   //     shellv[i] = NULL;
   // }
   // char* k = "$test1";
   // char* v = "HELLO";
   // char* k1 = "$this";
   // char* v1 = "WORLD";
   // char s[] = "$this is $test1";
   // insert_var(k,v);
   // insert_var(k1,v1);
   // printf("%d\n", find_var(k));
   // print_vars();
   // edit(s);

}
