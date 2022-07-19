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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "messageManager.hpp"
#include "clientManager.hpp"

#include <thread>
#include <iostream>
#include <csignal>

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

// Managers de mensagens e clientes conectados
MessageManager *msgMan;
ClientManager *clientMan;

struct arg_struct
{
    int socket;
    string ip;
};

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
 * @brief Função que checa se a string digitada contém apenas caracteres ASCII
 * 
 * @param s 
 * @return true 
 * @return false 
 */
bool isASCII(string &s)
{
    return !any_of(s.begin(), s.end(), [](char c)
                   { return static_cast<unsigned char>(c) > 127; });
}

/**
 * @brief Função utilizada para o recebimento de mensagens
 * Caso a mensagem seja maior do que 4096 será dividida e irá checar por flags de fim de mensagem
 * (no caso \\r\\n conforme consta na especificação do protocolo) para então ler e mostrar a mensagem dividida
 * 
 * Observação: Cada novo cliente conectado no servidor irá iniciar uma thread com a essa função para o recebimento de mensagens
 *
 * @param socket
 */
static void *receiveMessage(void *arg)
{
    char *rmBuffer = (char *)malloc(MAX * sizeof(char));

    struct arg_struct *auxSocket = (struct arg_struct *)arg;
    struct arg_struct socket;
    socket.socket = auxSocket->socket;
    socket.ip = auxSocket->ip;

    bool stopFlag = false;

    // Esperar até o usuário digitar seu nickname
    string str;
    string word;
    int flag = 0;

    while (flag < 1)
    {
        sendMessage(socket.socket, string(">> Por favor forneça seu nickname pelo comando /nickname <apelido desejado>"), MAX, NULL, NULL);
        read(socket.socket, rmBuffer, MAX);
        str = string(rmBuffer);
        istringstream ss(str);
        ss >> word;
        if (word != "/nickname")
            continue;
        if (ss >> word)
        {
            // Checando limite de caracteres e se todos são ASCII
            if (word.length() > 50 || !isASCII(word))
                continue;
            string aux = ">> Seu nick: " + word + "\n";
            sendMessage(socket.socket, aux, MAX, NULL, NULL);

            // Por fim, checar se já não existe um usuário com esse nome
            if(clientMan->checkUserName(word))
            {
                sendMessage(socket.socket, string(">> Nome de usuário já cadastrado!\n"), MAX, NULL, NULL);
                continue;
            }
            flag++;
        }
    }
    free(rmBuffer);
    rmBuffer = NULL;

    // Criar client
    Client client = clientMan->insertClient(socket.socket, socket.ip, word, "");

    // Pedir para o cliente entrar em uma sala
    rmBuffer = (char *)malloc(MAX * sizeof(char));
    flag = 0;
    while (flag < 1)
    {
        sendMessage(socket.socket, string(">> Por favor entre em uma sala pelo comando /join #<nome do canal>"), MAX, NULL, NULL);
        read(socket.socket, rmBuffer, MAX);
        str = string(rmBuffer);
        istringstream ss(str);
        ss >> word;
        if (word != "/join")
            continue;
        if (ss >> word)
        {
            // Checando se o comeco do nome tem os caracteres '#' ou '&',
            // e checando se ele possui um tamanho menor que 200 caracteres
            if (word.at(0) != '#' && word.at(0) != '&')
            {
                cout << ">> Nome invalido"
                     << "/n";
                continue;
            }
            if (word.length() >= 200)
            {
                cout << ">> Nome invalido"
                     << "/n";
                continue;
            }

            // Checando se em algum lugar da string ele possui os caracteres:
            // ctrl-G (ou ascii 7), ou ','
            bool check = true;
            for (int i = 0; i < word.length(); i++)
            {
                if (word.at(i) == 7 || word.at(i) == ',')
                {
                    cout << ">> Nome Invalido" << endl;
                    check = false;
                }
            }
            // Checando se não há mais palavras
            if (ss >> word || !check)
            {
                continue;
            }

            string aux = ">> Você entrou no canal: " + word + "\n";
            sendMessage(socket.socket, aux, MAX, NULL, NULL);
            
            flag++;
        }
    }
    free(rmBuffer);
    rmBuffer = NULL;

    // Colocar o cliente em uma sala
    if (!channelMan->isChannelActive(word))
    {
        // Canal não existe, criar o canal e adicionar o cliente nele
        channelMan->createChannel(word, &client);
        client.setChannelName(word);
    }
    else
    {
        // Canal já existe, colocar o cliente nele
        channelMan->insertClientChannel(word, &client);
        client.setChannelName(word);
    }

    while (client.getIsActive())
    {
        // Aloca o buffer para receber a mensagem
        rmBuffer = (char *)malloc(MAX * sizeof(char));

        while (true)
        {
            int ret = read(socket.socket, rmBuffer, MAX);

            // Caso o servidor/cliente enviar o comando /exit
            // O retorno do comando read() terá o código 0
            if (ret == 0)
            {
                free(rmBuffer);
                rmBuffer = NULL;
                client.setIsActive(false);
                break;
            }

            // Checar se há as flags de fim de mensagem, se não houver a mensagem é maior do que 4096 e será dividida em várias
            if (rmBuffer[MAX - 1] == '\n' && rmBuffer[MAX - 2] == '\r')
            {
                // Acabou
                string msg = string(rmBuffer);

                // Checar pelo comando ping
                istringstream ss(msg);
                ss >> word;
                if (word == "/ping")
                {
                    // Checando se não há mais palavras
                    if (ss >> word)
                    {
                        // Se houver, comando inválido, não fazer nada
                        continue;
                    }
                    sendMessage(socket.socket, string("Pong"), MAX, NULL, NULL);
                    flag++;
                }
                msg = msg + "\n";
                msgMan->recieveMessage(client, msg);
                break;
            }
            else
            {
                // Ainda há mensagens para enviar
                string msg = string(rmBuffer);
                msg = msg + "\n";
                msgMan->recieveMessage(client, msg);
            }
        }
        free(rmBuffer);
        rmBuffer = NULL;
    }

    // Usuário não está mais ativo
    channelMan->kickClient(client.getChannelName(), client.getNickname());
    clientMan->removeClient(client.getNickname());
    return NULL;
}

/**
 * @brief Função que aceita a conexão de novos clientes
 * Roda em um thread única, criando as threads de cada cliente que entra
 * @param arg 
 * @return void* 
 */
static void *acceptNewSockets(void *arg)
{
    // Struct de argumentos a serem enviados para a Thread de cada cliente
    struct arg_struct args;

    while (true)
    {
        args.socket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&addrLen);
        if (args.socket < 0)
        {
            cout << ">> Erro ao aceitar conexão!" << endl;
            exit(0);
        }
        else
        {
            cout << ">> Cliente aceito..." << endl
                 << endl;
        }
        // Pegar o IP do novo socket conectado
        args.ip = inet_ntoa(address.sin_addr);

        // Colocar o novo socket conectado na lista de sockets
        connectedSockets.push_back(args.socket);

        // Iniciar uma thread nova para ficar escutando esse socket
        pthread_t receiveMessageTh;

        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        // Thread para receber mensagens
        pthread_create(&receiveMessageTh, &attr, receiveMessage, &args);
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
        temp = NULL;
    }
    if (input != NULL)
    {
        free(input);
        input = NULL;
    }
}

int main()
{
    // signal(SIGINT, SIG_IGN);
    int connectionFd;
    int clientSockets[MAX_CLIENTS];

    for (size_t i = 0; i < MAX_CLIENTS; i++)
    {
        clientSockets[i] = 0;
    }

    // Criação do socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        cout << ">> Falha ao criar socket!" << endl;
        exit(0);
    }
    else
    {
        cout << ">> Socket criado com sucesso!" << endl;
    }

    // Associando IP e Porta para o servidor
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_aton(LOCALIP, &(address.sin_addr));

    // Associar o socket criado com o IP
    if ((bind(serverSocket, (struct sockaddr *)&address, sizeof(address))) != 0)
    {
        cout << ">> Erro ao associar o socket criado com o IP definido!" << endl;
        exit(0);
    }
    else
    {
        cout << ">> O socket foi associado ao IP definido com sucesso!" << endl;
    }

    // Agora, o servidor irá escutar por novas coneções
    // Até 5 conexões pendentes
    if ((listen(serverSocket, 5)) < 0)
    {
        cout << ">> Erro ao escutar na porta definida!" << endl;
        exit(0);
    }
    else
    {
        cout << ">> Escutando por conexões na porta definida..." << endl;
    }
    addrLen = sizeof(address);

    // Agora que a conexão está estabelecida o servidor fica ouvindo o cliente até que ele saia do chat
    // Detach Atribute for all threads
    pthread_t acceptNewSocketsTh;

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Thread para aceitar novas conexões
    int aux = 0;
    pthread_create(&acceptNewSocketsTh, &attr, acceptNewSockets, &aux);

    // Criar o messageManager
    msgMan = new MessageManager();
    msgMan->start();

    // Criar o clientManager
    clientMan = new ClientManager();

    // Depois de sair da função listenSocket encerrar a conexão
    string command;
    while (!exitSignal)
    {
    }
    msgMan->stop();

    // Checar se foi dado free em todos os buffers
    freeBuffers();
    close(serverSocket);
}