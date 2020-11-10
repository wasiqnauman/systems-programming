#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

extern int errno;

int main(int argc, char* argv[]) {
    if(argc < 3) {
        printf("Host or port not provided\n");
        exit(1);
    }
    char* server = argv[1];
    int port_number = atol(argv[2]);

    //step-1: Look up the server to get it's IP address
    struct hostent* record = gethostbyname(server);
    if(record == NULL) {
        herror("Unable to find host\n");
        exit(1);
    }

    struct in_addr* address_ptr = (struct in_addr *) record->h_addr;
    char* server_ip = inet_ntoa(*address_ptr);
    //step-2: Create a socket for communications
    
    int main_socket = socket(AF_INET, SOCK_STREAM,0);

    if(main_socket < 0) {
        perror("Unable to create socket\n");
        exit(1);
    }

    //step-3:create sockaddr_in struct to store server info
    
    struct sockaddr_in server_info;
    server_info.sin_len = sizeof(server_info);
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(port_number);
    server_info.sin_addr = *address_ptr;

    
    //step-4: connect to the server
    
    int r = connect(main_socket, (struct sockaddr*) &server_info, sizeof(server_info));
    if(r<0) {
        perror("Unable to connect to server\n");
        exit(1);
    }

    printf("Connected to server [%s]\n",server_ip);

    //step-5: create FILE* for communications with the server
    //              or
    //  you can use normal read() and write() to communicate with the server

    FILE* r_connection = fdopen(main_socket, "r");
    FILE* w_connection = fdopen(main_socket, "w");

    char* buf[1000];
    while(1) {
        char* s = fget(buf, 1000, r_connection);
        if(s == NULL) {
            printf("Disconnected\n");
            break;
        }
        printf("%s\n", buf);
    }

    fflush(w_connection);
    fclose(w_connection);
    fclose(r_connection);
    close(main_socket);
}
