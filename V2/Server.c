// Server
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections*/

// Function to count words in a string
int count_words(const char *str) {
    int count = 0;
    int in_word = 0;
    while (*str) {
        if (*str == ' ' || *str == '\n' || *str == '\t') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        str++;
    }
    return count;
}

int main (int argc, char **argv) {
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    // Create a socket for the server
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    // Preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // Bind the socket
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // Listen to the socket by creating a connection queue, then wait for clients
    listen (listenfd, LISTENQ);

    printf("%s\n","Server running...waiting for connections.");

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        // Accept a connection
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        printf("%s\n","Received request...");

        if ( (childpid = fork ()) == 0 ) { // If it’s 0, it’s child process
            printf ("%s\n","Child created for dealing with client requests");

            // Close listening socket
            close (listenfd);

            while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
                buf[n] = '\0';
                int word_count = count_words(buf);
                printf("String received: %s\n", buf);
                printf("Number of words: %d\n", word_count);
                snprintf(buf, MAXLINE, "Number of words: %d\n", word_count);
                send(connfd, buf, strlen(buf), 0);
            }

            if (n < 0)
                printf("%s\n", "Read error");
            exit(0);
        }
        // Close socket of the server
        close(connfd);
    }
}
