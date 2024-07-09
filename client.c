#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 4096 /* max text line length */
#define SERV_PORT 3000 /* port */

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    // Basic check of the arguments
    if (argc != 2) {
        perror("Usage: TCPClient <IP address of the server>");
        exit(1);
    }

    // Create a socket for the client
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    // Preparation of the socket address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT); // Convert to big-endian order

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(3);
    }

    printf("Enter a string: ");
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        send(sockfd, sendline, strlen(sendline), 0);

        if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
            // Error: server terminated prematurely
            perror("The server terminated prematurely");
            exit(4);
        }
        printf("String received from the server: ");
        fputs(recvline, stdout);
        printf("Enter a string: ");
    }

    exit(0);
}
