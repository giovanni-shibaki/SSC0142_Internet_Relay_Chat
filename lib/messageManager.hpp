/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

// Defines da assinatura da funcao de send
#define MAX 4096

#include <netinet/in.h>
#include <arpa/inet.h>

#include <bits/stdc++.h>
#include "message.hpp"
#include "channelManager.hpp"

// changing Library according to OS
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#define sleep usleep
#endif

using namespace std;

#include <semaphore.h>
#include <mutex>
#include <pthread.h>

// GLOBAL VARIABLES
sem_t notEmpty;
sem_t notFull;
mutex mtx;
queue<Message> messageQueue;
bool isServerActive = true;

ChannelManager *channelMan;

/**
 * @brief Função responsável pelo envio de mensagens por parte do servidor
 * Checa se a mensagem foi enviada com sucesso, caso contrário tenta reenviar até 5 vezes
 * Caso não consiga enviar nessas 5x, encerra a conexão com o cliente
 * 
 * @param socket 
 * @param msg 
 * @param size 
 * @param ch 
 * @param client
 * @return bool
 */
bool sendMessage(int socket, string msg, int size, Channel *ch, Client *client)
{
    bool flag = false;
    for(int i=0; i<5; i++)
    {
        send(socket, msg.c_str(), size, MSG_NOSIGNAL);

        char ret[MAX];
        if(read(socket, ret, MAX) == -1)
            break;
        string word;
        istringstream ss(ret);
        ss >> word;
        if(strcmp("<msgConfirm>", word.c_str()) == 0)
        {
            flag = true;
            break;
        }
        sleep(100);
    }
    if(!flag)
    {
        if(ch == NULL)
            return false;
        // Falha ao enviar mensagem, desconectar o cliente
        // Remover o usuário da sala
        cout << "Falha ao enviar mensagem, desconectando cliente: " << client->getNickname() << endl;
        ch->removeClient(client->getNickname());
        close(client->getSocketNumber());
        client->setIsActive(false);
        return false;
    }
    return true;
}

static void *sendMessage(void *arg)
{
    while (isServerActive)
    {
        string word;
        if (!messageQueue.empty())
        {
            mtx.lock();
            Message msg = messageQueue.front();
            messageQueue.pop();

            // Encontrar o canal em que o cliente está conectado
            Channel ch = channelMan->getChannel(msg.getClient()->getChannelName());

            // Checar se o usuário digitou algum comando
            if (msg.getMessage().at(0) == '/')
            {
                // Checar se o cliente é administrador
                if (msg.getClient()->getNickname() == ch.getAdminName())
                {
                    // Pode realizar os comandos
                    // Pegar a primeira palavra
                    istringstream ss(msg.getMessage());
                    ss >> word;
                    switch (word[1])
                    {
                    case 'k':
                        // Kick command
                        if (ss >> word)
                        {
                            bool flag = false;
                            for (Client *c : ch.getClients())
                            {
                                if (word == c->getNickname())
                                {
                                    // Remover o usuário da sala
                                    ch.removeClient(c->getNickname());

                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    if(!sendMessage(socket, string("Você foi desconectado do servidor!\n"), MAX, &ch, c))
                                    {
                                        // Falha ao enviar mensagem, cliente foi desconectado
                                        break;
                                    }
                                    close(c->getSocketNumber());
                                    c->setIsActive(false);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                sendMessage(msg.getClient()->getSocketNumber(), string("Usuário não encontrado!\n"), MAX, &ch, msg.getClient());
                            }
                        }
                        else
                        {
                            sendMessage(msg.getClient()->getSocketNumber(), string("Faltou algum argumento, comando inválido!\n"), MAX, &ch, msg.getClient());
                        }
                        break;
                    case 'm':
                        // Mute command
                        if (ss >> word)
                        {
                            bool flag = false;
                            for (Client *c : ch.getClients())
                            {
                                if (word == c->getNickname())
                                {
                                    // Se o usuário já está mutado
                                    if (c->getIsMuted())
                                    {
                                        sendMessage(msg.getClient()->getSocketNumber(), string("O usuário já se encontra mutado!\n"), MAX, &ch, msg.getClient());
                                        flag = true;
                                        break;
                                    }

                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    c->mute();
                                    sendMessage(socket, string("Você foi mutado pelo administrador do canal!\n"), MAX, &ch, c);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                sendMessage(msg.getClient()->getSocketNumber(), string("Usuário não encontrado!\n"), MAX, &ch, msg.getClient());
                            }
                        }
                        else
                        {
                            sendMessage(msg.getClient()->getSocketNumber(), string("Faltou algum argumento, comando inválido!\n"), MAX, &ch, msg.getClient());
                        }
                        break;
                    case 'u':
                        // Unmute command
                        if (ss >> word)
                        {
                            bool flag = false;
                            for (Client *c : ch.getClients())
                            {
                                // Verificar se o cliente não é o mesmo que enviou a mensagem
                                if (word == c->getNickname())
                                {
                                    // Se o usuário já está desmutado
                                    if (!c->getIsMuted())
                                    {
                                        sendMessage(msg.getClient()->getSocketNumber(), string("O usuário já se encontra desmutado!\n"), MAX, &ch, msg.getClient());
                                        flag = true;
                                        break;
                                    }

                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    c->unmute();
                                    sendMessage(socket, string("Você foi desmutado pelo administrador do canal!\n"), MAX, &ch, c);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                sendMessage(msg.getClient()->getSocketNumber(), string("Usuário não encontrado!\n"), MAX, &ch, msg.getClient());
                            }
                        }
                        else
                        {
                            sendMessage(msg.getClient()->getSocketNumber(), string("Faltou algum argumento, comando inválido!\n"), MAX, &ch, msg.getClient());
                        }
                        break;
                    case 'w':
                        // Whois command
                        if (ss >> word)
                        {
                            bool flag = false;
                            for (Client *c : ch.getClients())
                            {
                                // Verificar se o cliente não é o mesmo que enviou a mensagem
                                if (word == c->getNickname())
                                {
                                    // Enviar a mensagem para o usuário
                                    string aux = "Ip de " + c->getNickname() + " é: " + c->getIp() + "\n";
                                    sendMessage(msg.getClient()->getSocketNumber(), aux, MAX, &ch, msg.getClient());
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                sendMessage(msg.getClient()->getSocketNumber(), string("Usuário não encontrado!\n"), MAX, &ch, msg.getClient());
                            }
                        }
                        else
                        {
                            sendMessage(msg.getClient()->getSocketNumber(), string("Faltou algum argumento, comando inválido!\n"), MAX, &ch, msg.getClient());
                        }
                        break;
                    default:
                        sendMessage(msg.getClient()->getSocketNumber(), string("Comando iválido!\n"), MAX, &ch, msg.getClient());
                        break;
                    }
                }
                else
                {
                    // Não pode realizar os comandos caso não seja administrador
                    sendMessage(msg.getClient()->getSocketNumber(), string("Comando disponível apenas para o administrador do canal!\n"), MAX, &ch, msg.getClient());
                }
            }
            else
            {
                // Checar se o usuário que mandou a mensagem não está mutado
                if (msg.getClient()->getIsMuted())
                {
                    // Se estiver mutado apenas avisa que ele está mutado e não faz nada
                    sendMessage(msg.getClient()->getSocketNumber(), string("Você está mutado!\n"), MAX, &ch, msg.getClient());
                }
                else
                {
                    for (Client *c : ch.getClients())
                    {
                        // Verificar se o cliente não é o mesmo que enviou a mensagem
                        if (msg.getClient()->getNickname() == c->getNickname())
                            continue;

                        // Enviar a mensagem para o usuário
                        int socket = c->getSocketNumber();
                        string aux = msg.getClient()->getNickname() + ": " + msg.getMessage();
                        sendMessage(socket, aux, MAX, &ch, c);
                    }
                }
            }
            mtx.unlock();
            sleep(500);
        }
    }
    return NULL;
}

class MessageManager
{
private:
    // Produtor irá receber as novas mensagens enviadas pelos clientes
    // Consumidor irá enviar a mensagem para todos os clientes que estão no mesmo chat do cliente que enviou
    pthread_t consumer;

public:
    MessageManager()
    {
        channelMan = new ChannelManager();
    }

    void start()
    {
        pthread_attr_t attr;

        sem_init(&notEmpty, 0, 0);
        sem_init(&notFull, 0, 100);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        int r;

        r = pthread_create(&this->consumer, &attr, sendMessage, NULL);
        if (r)
        {
            cout << "Error in creating thread" << endl;
            exit(-1);
        }

        pthread_attr_destroy(&attr);
    }

    void stop()
    {
        isServerActive = false;
    }

    void recieveMessage(Client c, string msg)
    {
        mtx.lock();

        // Inserir a mensagem na Queue de mensagens
        Message message = Message(c, msg);
        messageQueue.push(message);

        mtx.unlock();
        sleep(500);
    }
};

#endif