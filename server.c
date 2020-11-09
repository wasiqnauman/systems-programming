#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
extern int errno;
int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Please provide a port number\n");
        exit(1);
    }
    int port_number = atol(argv[1]);
    printf("Starting server on port: %d\n", port_number);
    // create the main socket
    int main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(main_socket < 0) {
        fprintf(stderr, "Unable to create main_socket\n");
        exit(1);
    }

    struct sockaddr_in server_info;
    server_info.sin_len = sizeof(server_info);
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = htonl(INADDR_ANY);
    server_info.sin_port = htons(port_number);

    int r1 = bind(main_socket, (struct sockaddr_in *) &server_info, sizeof(server_info));
    if(r1 < 0) {
        fprintf(stderr, "Port number requested is in use\n");
        exit(1);
    }

    int r2 = listen(main_socket, 3);
    if(r2 < 0) {
        fprintf(stderr, "Unable to listen on port %d \n", port_number);
        exit(1);
    
    }
    printf("Listening on port: %d\n", port_number);
    int num_session = 0;
    while(1) {
        struct sockaddr_in client_info;
        int client_info_size = sizeof(client_info);

        int session_socket = accept(main_socket, (struct sockaddr_in *) &client_info, &client_info_size);
        if(session_socket < 0) {
            if(errno == EINTR) continue; // the process was interupted by a signal
            perror("accept");
            usleep(100000);
            continue;
        }
        num_session +=1;

        char* who = inet_ntoa(client_info.sin_addr);
        FILE* r_connection = fdopen(session_socket, "r");
        FILE* w_connection = fdopen(session_socket, "w");
        
        fprintf(w_connection, "Hello [%s] welcome to my server!\n", who);

        fflush(w_connection);
        fclose(w_connection);
        fclose(r_connection);
        close(session_socket);
    }
}
