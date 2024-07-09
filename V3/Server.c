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

// Function to find the longest word in a string
void find_longest_word(const char *str, char *longest_word) {
    int max_length = 0;
    int length = 0;
    const char *start = str;
    const char *word_start = NULL;

    while (*str) {
        if (*str == ' ' || *str == '\n' || *str == '\t' || *str == '\0') {
            if (length > max_length) {
                max_length = length;
                word_start = start;
            }
            length = 0;
            start = str + 1;
        } else {
            length++;
        }
        str++;
    }
    if (length > max_length) {
        max_length = length;
        word_start = start;
    }
    if (word_start) {
        strncpy(longest_word, word_start, max_length);
        longest_word[max_length] = '\0';
    }
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
                char longest_word[MAXLINE];
                find_longest_word(buf, longest_word);
                printf("String received: %s\n", buf);
                printf("Longest word: %s\n", longest_word);
                snprintf(buf, MAXLINE, "Longest word: %s\n", longest_word);
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
