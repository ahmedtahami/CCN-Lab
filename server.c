#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096 /* max text line length */
#define SERV_PORT 3000 /* port */
#define LISTENQ 8 /* maximum number of client connections */

// Placeholder function for processing received strings
void process_string(const char *input, char *output) {
    // Example: Echo the input string
    snprintf(output, MAXLINE, "Processed: %s", input);
}

int main(int argc, char **argv) {
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    // Create a socket for the server
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    // Preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // Bind the socket
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // Listen to the socket by creating a connection queue, then wait for clients
    listen(listenfd, LISTENQ);

    printf("Server running...waiting for connections.\n");

    for (;;) {
        clilen = sizeof(cliaddr);
        // Accept a connection
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

        printf("Received request...\n");

        if ((childpid = fork()) == 0) { // If it’s 0, it’s child process
            printf("Child created for dealing with client requests\n");

            // Close listening socket
            close(listenfd);

            while ((n = recv(connfd, buf, MAXLINE, 0)) > 0) {
                buf[n] = '\0';
                char output[MAXLINE];
                process_string(buf, output);
                printf("String received: %s\n", buf);
                printf("Sending response: %s\n", output);
                send(connfd, output, strlen(output), 0);
            }

            if (n < 0) {
                printf("Read error\n");
            }
            exit(0);
        }
        // Close socket of the server
        close(connfd);
    }
}
