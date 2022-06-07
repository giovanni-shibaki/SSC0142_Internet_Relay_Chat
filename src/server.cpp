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
#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>

#include <thread>
#include <mutex>

#define TRUE 1
#define FALSE 0

#define PORT 42069
#define LOCALIP "127.0.0.1"
#define MAX 4096
#define MAX_CLIENTS 10
#define READLINE_BUFFER 4096 // Buffer utilizado na função readline();

int serverSocket;
int addrLen;

using namespace std;

// Struct que guarda as informações sobre as conexões
struct sockaddr_in address;

/**
 * @brief Le uma linha ate o \0
 *
 * @param stream
 * @return char*
 */
char *readLine(FILE *stream)
{
       char *string = NULL;
       int pont = 0;
       do
       {
              if (pont % READLINE_BUFFER == 0) // Se o ponteiro for divisivel pelo buffer ou é 0 é preciso alocar mais memória HEAP para receber a string
              {
                     string = (char *)realloc(string, ((pont / READLINE_BUFFER) + 1) * READLINE_BUFFER);
              }
              string[pont] = (char)fgetc(stream);                             // Recebe um dos caracteres da string
       } while (string[pont++] != '\n' && !feof(stream));                     // A condição de parada é achar o \n ou encontrar o marcador de fim de arquivo
       string[pont - 1] = '\0';                                               // Insere o terminador de string
       string = (char *)realloc(string, (strlen(string) + 1) * sizeof(char)); // Ajusta o tamanho da memória para exatamente o tamanho da string recebida
       return string;
}

/**
 * @brief Função utilizada para o recebimento de mensagens
 * Caso a mensagem seja maior do que 4096 será dividida e irá checar por flags de fim de mensagem
 * (no caso \r\n conforme consta na especificação do protocolo) para então ler e mostrar a mensagem dividida
 * 
 * @param socket 
 */
void receiveMessage(int socket)
{
       char *buffer = NULL;

       // Aloca o buffer para receber a mensagem
       buffer = (char *)malloc(MAX * sizeof(char));

       cout << "Mensagem recebida do cliente: ";
       while (true)
       {
              read(socket, buffer, MAX);
              // Checar se há as flags de fim de mensagem, se não houver a mensagem é maior do que 4096 e será dividida em várias
              if (buffer[MAX - 1] == '\n' && buffer[MAX - 2] == '\r')
              {
                     // Acabou
                     cout << buffer << endl;
                     break;
              }
              else
              {
                     // Ainda há mensagens para enviar
                     cout << buffer;
              }
       }
       free(buffer);
       buffer = NULL;
}

/**
 * @brief Função utilizada para o envio de mensagens
 * Caso a mensagem tenha tamanho maior que 4096, irá divir em várias mensagens
 * 
 * @param socket 
 */
void sendMessage(int socket)
{
       char *buffer = NULL;
       cout << "Digite a mensagem a ser enviada: ";
       buffer = readLine(stdin);

       // Verificar se o tamanho da mensagem enviada é maior do que 4096, se for maior
       // deve dividir a mensagem e enviar em partes separadas de até 4096 caracteres

       int numVezes = ((strlen(buffer) + 1) / 4096) + 1;
       for (int i = 0; i < numVezes; i++)
       {
              // Dividir a mensagem em um bloco de até 4096 caracteres
              char *temp = (char *)malloc(MAX * sizeof(char));

              // Checar se não é a última mensagem da sequencia
              if ((i + 1) >= numVezes)
              {
                     int num = strlen(&buffer[i * (MAX)]) + 1;
                     strncpy(temp, &buffer[i * (MAX)], num);

                     // Adicionar o \r\n indicando a finalização
                     temp[MAX - 2] = '\r';
                     temp[MAX - 1] = '\n';
              }
              else
              {
                     strncpy(temp, &buffer[i * (MAX)], MAX);
              }
              send(socket, temp, MAX, MSG_NOSIGNAL);

              free(temp);
              temp = NULL;
       }
       free(buffer);
       buffer = NULL;
}

/**
 * @brief Função que fica ouvindo o cliente no socket
 * OBS: Ficará na classe clientManager, onde cada cliente terá uma thread que ficará escutando as suas mensagens
 */
void listenSocket(int socket)
{
       char *buffer = NULL;
       int n;

       while (TRUE)
       {
              // Recebendo a mensagem
              receiveMessage(socket);

              // Enviando a mensagem
              sendMessage(socket);
       }
}

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
              cout << "Erro ao aceitar conexão!" << endl;
              exit(0);
       }
       else
       {
              cout << "Cliente aceito..." << endl << endl;
       }
       return newSocket;
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
              cout << "Falha ao criar socket!" << endl;
              exit(0);
       }
       else
       {
              cout << "Socket criado com sucesso!" << endl;
       }

       // Associando IP e Porta para o servidor
       address.sin_family = AF_INET;
       address.sin_port = htons(PORT);
       inet_aton(LOCALIP, &(address.sin_addr));

       // Associar o socket criado com o IP
       if ((bind(serverSocket, (struct sockaddr *)&address, sizeof(address))) != 0)
       {
              cout << "Erro ao associar o socket criado com o IP definido!" << endl;
              exit(0);
       }
       else
       {
              cout << "O socket foi associado ao IP definido com sucesso!" << endl;
       }

       // Agora, o servidor irá escutar por novas coneções
       // Até 5 conexões pendentes
       if ((listen(serverSocket, 5)) < 0)
       {
              cout << "Erro ao escutar na porta definida!" << endl;
              exit(0);
       }
       else
       {
              cout << "Escutando por conexões na porta definida..." << endl;
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