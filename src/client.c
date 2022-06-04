/**
 * @file main.c
 * @author 11796444 - Giovanni Shibaki Camargo 
 * @author 11796472 - Lucas Keiti Anbo Mihara
 * @author 11795589 - Vitor Caetano Brustolin
 * @version 0.1
 * @date 2022-05-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <mutex>

/**
 * @brief Descrição da função
 *
 * @param parametroDaFuncao
 */
void funcao()
{
    
}

int main()
{
   
}
#include <sys/socket.h>
#include <sys/types.h>


/**
 * @brief Descrição da função
 *
 * @param parametroDaFuncao
 */
void funcao()
{
    
}

int main()
{
   
}


// #include <netdb.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #define MAX 80
// #define PORT 8080
// #define SA struct sockaddr
// void func(int sockfd)
// {
//     char buff[MAX];
//     int n;
//     for (;;) {
//         bzero(buff, sizeof(buff));
//         printf("Enter the string : ");
//         n = 0;
//         while ((buff[n++] = getchar()) != '\n')
//             ;
//         write(sockfd, buff, sizeof(buff));
//         bzero(buff, sizeof(buff));
//         read(sockfd, buff, sizeof(buff));
//         printf("From Server : %s", buff);
//         if ((strncmp(buff, "exit", 4)) == 0) {
//             printf("Client Exit...\n");
//             break;
//         }
//     }
// }
   
// int main()
// {
//     int sockfd, connfd;
//     struct sockaddr_in servaddr, cli;
   
//     // socket create and verification
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd == -1) {
//         printf("socket creation failed...\n");
//         exit(0);
//     }
//     else
//         printf("Socket successfully created..\n");
//     bzero(&servaddr, sizeof(servaddr));
   
//     // assign IP, PORT
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//     servaddr.sin_port = htons(PORT);
   
//     // connect the client socket to server socket
//     if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
//         printf("connection with the server failed...\n");
//         exit(0);
//     }
//     else
//         printf("connected to the server..\n");
   
//     // function for chat
//     func(sockfd);
   
//     // close the socket
//     close(sockfd);
// }