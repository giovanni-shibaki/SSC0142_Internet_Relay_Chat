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
#include <math.h>

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#define TRUE 1
#define FALSE 0

#define PORT 42069
#define LOCALIP "127.0.0.1"
#define MAX 4096
#define MAX_CLIENTS 10
#define READLINE_BUFFER 4096 // Buffer utilizado na função readline();

using namespace std;

int clientSocket;
int addrLen;

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
        string[pont] = (char)fgetc(stream);                                // Recebe um dos caracteres da string
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

    cout << "Mensagem recebida do servidor: ";
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

void listenSocket(int socket)
{
    char *buffer = NULL;
    int n;

    while (TRUE)
    {
        // Enviando a mensagem
        sendMessage(socket);

        // Recebendo a mensagem
        receiveMessage(socket);
    }
}

int main()
{
    int connectionFd;

    // Criação do socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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
    if ((connect(clientSocket, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        cout << "Erro ao associar o socket criado com o IP definido!" << endl;
        exit(0);
    }
    else
    {
        cout << "O socket foi associado ao IP definido com sucesso!" << endl << endl;
    }

    listenSocket(clientSocket);

    // Depois de sair da função listenSocket encerrar a conexão
    close(clientSocket);
}