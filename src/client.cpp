/**
 * @file client.cpp
 * @author 11796444 - Giovanni Shibaki Camargo
 * @author 11796472 - Lucas Keiti Anbo Mihara
 * @author 11795589 - Vitor Caetano Brustolin
 * @brief
 * @version 0.1
 * @date 2022-06-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#define TRUE 1
#define FALSE 0

#define PORT 3002
#define LOCALIP "127.0.0.1"
#define MAX 4096
#define MAX_CLIENTS 10

using namespace std;

int clientSocket;
int addrLen;

// Struct que guarda as informações sobre as conexões
struct sockaddr_in address;

/**
 * @brief Função do cliente para mandar mensagens ao servidor
 *
 */
void listenSocket(int socket)
{
    char message[MAX];
    int n;

    while (TRUE)
    {
        // Zerando o message para receber uma nova mensagem
        bzero(message, MAX);

        printf("Digite a mensagem a ser enviada: ");
        n = 0;

        // Ler a mensagem a ser enviada
        cin >> message;

        // Enviar a mensagem para o servidor conectado
        send(clientSocket, message, sizeof(message), MSG_NOSIGNAL);
        bzero(message, MAX);

        // Recebe a mensagem de volta do servidor
        cout << "Esperando resposta do servidor..." << endl;
        read(clientSocket, message, sizeof(message));

        cout << "Mensagem recebida do servidor: " << message << endl;
        if ((strncmp(message, "exit", 4)) == 0) 
        {
            printf("Client exit...\n");
            break;
        }
    }
}

int main()
{
    int connectionFd;

    // Criação do socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Falha ao criar socket!\n");
        exit(0);
    }
    else
    {
        printf("Socket criado com sucesso!\n");
    }

    // Limpar a variável myself para inclusão do IP e da porta
    //bzero(&clientSocket, sizeof(clientSocket));

    // Associando IP e Porta para o servidor
    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);
    inet_aton(LOCALIP, &(address.sin_addr));

    // Associar o socket criado com o IP
    if ((connect(clientSocket, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        printf("Erro ao associar o socket criado com o IP definido!\n");
        exit(0);
    }
    else
    {
        printf("O socket foi associado ao IP definido com sucesso!\n");
    }

    listenSocket(clientSocket);

    // Depois de sair da função listenSocket encerrar a conexão
    close(clientSocket);
}

// O limite para o tamanho de cada mensagem deve ser de 4096 caracteres. Caso um usuário envie
// uma mensagem maior do que isso ela deverá ser dividida em múltiplas mensagens automaticamente.

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