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
#include <vector>
#include <queue>

#define TRUE 1
#define FALSE 0

#define PORT 42069
#define LOCALIP "127.0.0.1"
#define MAX 4096
#define MAX_CLIENTS 10
#define READLINE_BUFFER 4096 // Buffer utilizado na função readline();

using namespace std;

int serverSocket;
int addrLen;
bool exitSignal = false;

char *smBuffer = NULL;
char *rmBuffer = NULL;
char *temp = NULL;
char *input = NULL;

// Struct que guarda as informações sobre as conexões
struct sockaddr_in address;

// Vetor de sockets conectados
vector<int> connectedSockets;

// Queue de mensagens
queue<string> messageQueue;

/**
 * @brief Le uma linha ate o \0
 *
 * @param stream
 * @return char*
 */
char *readLine(FILE *stream)
{
    input = NULL;
    int pont = 0;
    do
    {
        if (pont % READLINE_BUFFER == 0) // Se o ponteiro for divisivel pelo buffer ou é 0 é preciso alocar mais memória HEAP para receber a string
        {
            input = (char *)realloc(input, ((pont / READLINE_BUFFER) + 1) * READLINE_BUFFER);
        }
        input[pont] = (char)fgetc(stream);            // Recebe um dos caracteres da string
    } while (input[pont++] != '\n' && !feof(stream)); // A condição de parada é achar o \n ou encontrar o marcador de fim de arquivo
    if (input[0] != -1)
        input[pont - 1] = '\0';                                         // Insere o terminador de string
    input = (char *)realloc(input, (strlen(input) + 1) * sizeof(char)); // Ajusta o tamanho da memória para exatamente o tamanho da string recebida
    return input;
}

/**
 * @brief Função utilizada para o recebimento de mensagens
 * Caso a mensagem seja maior do que 4096 será dividida e irá checar por flags de fim de mensagem
 * (no caso \r\n conforme consta na especificação do protocolo) para então ler e mostrar a mensagem dividida
 *
 * @param socket
 */
static void *receiveMessage(void *arg)
{
    int *socket = (int *)arg;
    bool stopFlag = false;
    while (!stopFlag)
    {
        // Aloca o buffer para receber a mensagem
        rmBuffer = (char *)malloc(MAX * sizeof(char));

        while (true)
        {
            int ret = read((int)*socket, rmBuffer, MAX);

            // Caso o servidor/cliente enviar o comando /exit
            // O retorno do comando read() terá o código 0
            if (ret == 0)
            {
                free(rmBuffer);
                rmBuffer = NULL;
                stopFlag = true;
                break;
            }

            cout << ">> ";

            // Checar se há as flags de fim de mensagem, se não houver a mensagem é maior do que 4096 e será dividida em várias
            if (rmBuffer[MAX - 1] == '\n' && rmBuffer[MAX - 2] == '\r')
            {
                // Acabou
                string msg = string(rmBuffer);
                msg = msg + "\n";
                cout << msg << endl;
                messageQueue.push(msg);
                //cout << rmBuffer << endl;
                break;
            }
            else
            {
                // Ainda há mensagens para enviar
                string msg = string(rmBuffer);
                msg = msg + "\n";
                cout << msg << endl;
                messageQueue.push(msg);
                //cout << rmBuffer << endl;
            }
        }
        free(rmBuffer);
        rmBuffer = NULL;
    }

    //exitSignal = true;
    return NULL;
}

/**
 * @brief Função utilizada para o envio de mensagens
 * Caso a mensagem tenha tamanho maior que 4096, irá divir em várias mensagens
 *
 * @param socket
 */
static void *sendMessage(void *arg)
{
    while (true)
    {
        // Verificar se há mensagens
        if(messageQueue.empty())
        {
            continue;
        }

        /*if (strcmp(smBuffer, "/exit") == 0 || smBuffer[0] == -1)
        {
            free(smBuffer);
            smBuffer = NULL;
            break;
        }*/

        string msg = messageQueue.front();
        messageQueue.pop();

        // Verificar se o tamanho da mensagem enviada é maior do que 4096, se for maior
        // deve dividir a mensagem e enviar em partes separadas de até 4096 caracteres

        int numVezes = ((msg.length() + 1) / 4096) + 1;
        for (int i = 0; i < numVezes; i++)
        {
            // Dividir a mensagem em um bloco de até 4096 caracteres
            temp = (char *)malloc(MAX * sizeof(char));

            // Checar se não é a última mensagem da sequencia
            if ((i + 1) >= numVezes)
            {
                int num = strlen(&msg[i * (MAX)]) + 1;
                strncpy(temp, &msg[i * (MAX)], num);

                // Adicionar o \r\n indicando a finalização
                temp[MAX - 2] = '\r';
                temp[MAX - 1] = '\n';
            }
            else
            {
                strncpy(temp, &msg[i * (MAX)], MAX);
            }
            // Mandar para todos os sockets connectados (por enquanto)
            for(int socket : connectedSockets)
            {
                send(socket, temp, MAX, MSG_NOSIGNAL);
            }

            free(temp);
            temp = NULL;
        }
        /*free(smBuffer);
        smBuffer = NULL;*/
    }

    //exitSignal = true;
    return NULL;
}

/**
 * @brief
 *
 * @param arg
 * @return void*
 */
static void *acceptNewSockets(void *arg)
{
    int newSocket; // Variável utilizada para aceitar novas conexões de sockets cliente
    while (true)
    {
        newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&addrLen);
        if (newSocket < 0)
        {
            cout << "Erro ao aceitar conexão!" << endl;
            exit(0);
        }
        else
        {
            cout << "Cliente aceito..." << endl
                 << endl;
        }

        // Colocar o novo socket conectado na lista de sockets
        connectedSockets.push_back(newSocket);

        // Iniciar uma thread nova para ficar escutando esse socket
        pthread_t receiveMessageTh;

        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        // Thread para receber mensagens
        pthread_create(&receiveMessageTh, &attr, receiveMessage, &newSocket);
    }

    return NULL;
}

/**
 * @brief Função que da free em todos os buffers utilizados para recebimento e envio de mensagens
 * É utilizada ao finalizar a aplicação
 */
void freeBuffers()
{
    if (rmBuffer != NULL)
    {
        free(rmBuffer);
        rmBuffer = NULL;
    }
    if (smBuffer != NULL)
    {
        free(smBuffer);
        smBuffer = NULL;
    }
    if (temp != NULL)
    {
        free(temp);
        smBuffer = NULL;
    }
    if (input != NULL)
    {
        free(input);
        input = NULL;
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

    // Agora que a conexão está estabelecida o servidor fica ouvindo o cliente até que ele saia do chat
    // Detach Atribute for all threads
    pthread_t acceptNewSocketsTh;
    pthread_t sendMessageTh;

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Thread para aceitar novas conexões
    int aux = 0;
    pthread_create(&acceptNewSocketsTh, &attr, acceptNewSockets, &aux);

    // Thread para enviar mensagens
    pthread_create(&sendMessageTh, &attr, sendMessage, NULL);

    // Depois de sair da função listenSocket encerrar a conexão
    while (!exitSignal)
    {
    }

    // Checar se foi dado free em todos os buffers
    freeBuffers();
    close(serverSocket);
}