/**
 * @file server.cpp
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

#include <thread>
#include <mutex>

#define TRUE 1
#define FALSE 0

#define PORT 3002
#define LOCALIP "127.0.0.1"
#define MAX 4096
#define MAX_CLIENTS 10

int serverSocket;
int addrLen;

using namespace std;

// Struct que guarda as informações sobre as conexões
struct sockaddr_in address;

/**
 * @brief Função que aceita novas conexões no socket
 * 
 * @return int 
 */
int acceptNewSockets()
{
       int newSocket; // Variável utilizada para aceitar novas conexões de sockets cliente
       newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&addrLen);
       if (newSocket < 0)
       {
              printf("Erro ao aceitar conexão!\n");
              exit(0);
       }
       else
       {
              printf("Cliente aceito...\n");
       }
       return newSocket;
}

/**
 * @brief Função que fica ouvindo o cliente no socket
 * OBS: Ficará na classe clientManager, onde cada cliente terá uma thread que ficará escutando as suas mensagens
 */
void listenSocket(int socket)
{
       char buffer[MAX];
       int n;

       while(TRUE)
       {
              // Zerando o buffer para receber uma nova mensagem
              bzero(buffer, MAX);

              // Ler a mensagem do cliente e colocar no buffer
              read(socket, buffer, sizeof(buffer));

              printf("Mensagem recebida do cliente: %s\n", buffer);
              bzero(buffer, MAX);
              n = 0;

              printf("Digite sua resposta ao cliente: ");
              while((buffer[n++] = getchar()) != '\n');

              // Mandar uma mensagem de resposta para o usuário
              send(socket, buffer, sizeof(buffer), MSG_NOSIGNAL);

              if(strncmp("exit", buffer, 4) == 0)
              {
                     printf("Server exit...\n");
                     break;
              }
       }
}

int main()
{
       int connectionFd;
       int clientSockets[MAX_CLIENTS];

       for (size_t i = 0; i < MAX_CLIENTS; i++)
       {
              clientSockets[i] = 0;
       }

       // Criação do socket
       if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
       {
              printf("Falha ao criar socket!\n");
              exit(0);
       }
       else
       {
              printf("Socket criado com sucesso!\n");
       }

       // Permitindo que o socket do servidor aceite multiplas conexões
       /*if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)TRUE, sizeof(TRUE)) < 0)
       {
              printf("Falha ao configurar socket!\n");
              exit(0);
       }*/

       // Limpar a variável myself para inclusão do IP e da porta
       //bzero(&serverSocket, sizeof(serverSocket));

       // Associando IP e Porta para o servidor
       address.sin_family = AF_INET;
       //address.sin_addr.s_addr = htonl(INADDR_ANY);
       address.sin_port = htons(PORT);
       inet_aton(LOCALIP, &(address.sin_addr));
       // Associar o socket criado com o IP
       if ((bind(serverSocket, (struct sockaddr *)&address, sizeof(address))) != 0)
       {
              printf("Erro ao associar o socket criado com o IP definido!\n");
              exit(0);
       }
       else
       {
              printf("O socket foi associado ao IP definido com sucesso!\n");
       }

       // Agora, o servidor irá escutar por novas coneções
       // Até 5 conexões pendentes
       if ((listen(serverSocket, 5)) < 0)
       {
              printf("Erro ao escutar na porta definida!\n");
              exit(0);
       }
       else
       {
              printf("Escutando por conexões na porta definida...\n");
       }
       addrLen = sizeof(address);

       // Para aceitar várias conexões colocar isso em uma thread que roda em loop ou apenas um loop aqui

       // Função temporária que aceita uma nova conexão
       int acceptedSocket = acceptNewSockets();

       // Agora que a conexão está estabelecida o servidor fica ouvindo o cliente até que ele saia do chat
       // TODO: Cada cliente conectado terá a sua thread própria
       listenSocket(acceptedSocket);

       // Depois de sair da função listenSocket encerrar a conexão
       close(serverSocket);
}

// Criar uma struct com as informações de cada cliente
// Criar uma struct com as informações de cada canal de voz

// Cada cliente conectado irá ficar em uma thread própria ouvindo o que ele fala
// Quando ele fala, verificar se é um comando, se não for um comando, mandar a mensagem para todos
// os clientes que estão no mesmo canal dele

// Thread específica para o envio das mensagens, que pega as mensagens dentro da lista de mensagens
// e envia para todos os usuários que estão conectados no mesmo canal

// #include <stdio.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #define MAX 80
// #define PORT 8080
// #define SA struct sockaddr

// // Function designed for chat between client and server.
// void func(int connfd)
// {
//     char buff[MAX];
//     int n;
//     // infinite loop for chat
//     for (;;) {
//         bzero(buff, MAX);

//         // read the message from client and copy it in buffer
//         read(connfd, buff, sizeof(buff));
//         // print buffer which contains the client contents
//         printf("From client: %s\t To client : ", buff);
//         bzero(buff, MAX);
//         n = 0;
//         // copy server message in the buffer
//         while ((buff[n++] = getchar()) != '\n')
//             ;

//         // and send that buffer to client
//         write(connfd, buff, sizeof(buff));

//         // if msg contains "Exit" then server exit and chat ended.
//         if (strncmp("exit", buff, 4) == 0) {
//             printf("Server Exit...\n");
//             break;
//         }
//     }
// }

// // Driver function
// int main()
// {
//     int sockfd, connfd, len;
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
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servaddr.sin_port = htons(PORT);

//     // Binding newly created socket to given IP and verification
//     if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
//         printf("socket bind failed...\n");
//         exit(0);
//     }
//     else
//         printf("Socket successfully binded..\n");

//     // Now server is ready to listen and verification
//     if ((listen(sockfd, 5)) != 0) {
//         printf("Listen failed...\n");
//         exit(0);
//     }
//     else
//         printf("Server listening..\n");
//     len = sizeof(cli);

//     // Accept the data packet from client and verification

// As informações do socket do cliente conectado fica nesse int conffd
//     connfd = accept(sockfd, (SA*)&cli, &len);
//     if (connfd < 0) {
//         printf("server accept failed...\n");
//         exit(0);
//     }
//     else
//         printf("server accept the client...\n");

//     // Function for chatting between client and server
//     func(connfd);

//     // After chatting close the socket
//     close(sockfd);
// }