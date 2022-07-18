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
            Channel ch = channelMan->getChannel(msg.getClient().getChannelName());

            // Usar o ChannelManager para enviar mensagem para todos os clientes que estão na mesma sala do que enviou a mensagem
            // Passo a passo:
            // Checar se o usuário digitou algum comando
            if (msg.getMessage().at(0) == '/')
            { // Digitou algum dos comandos abaixo:
                // /kick nomeUsuario
                // /mute nomeUsuario
                // /unmute nomeUsuario
                // /whois nomeUsuario
                // Checar se o cliente é administrador
                if (msg.getClient().getNickname() == ch.getAdminName())
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
                            for (Client *c : ch.getClients())
                            {
                                if (word == c->getNickname())
                                {
                                    // Remover o usuário da sala
                                    ch.removeClient(c->getNickname());

                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    send(socket, "Você foi desconectado do servidor!\n", MAX, MSG_NOSIGNAL);
                                    close(c->getSocketNumber());
                                    c->setIsActive(false);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            send(msg.getClient().getSocketNumber(), "Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                        break;
                    case 'm':
                        // Mute command
                        if (ss >> word)
                        {
                            for (Client *c : ch.getClients())
                            {
                                if (word == c->getNickname())
                                {
                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    c->mute();
                                    send(socket, "Você foi mutado pelo administrador do canal!\n", MAX, MSG_NOSIGNAL);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            send(msg.getClient().getSocketNumber(), "Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                        break;
                    case 'u':
                        // Unmute command
                        if (ss >> word)
                        {
                            for (Client *c : ch.getClients())
                            {
                                // Verificar se o cliente não é o mesmo que enviou a mensagem
                                if (word == c->getNickname())
                                {
                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    c->unmute();
                                    send(socket, "Você foi desmutado pelo administrador do canal!\n", MAX, MSG_NOSIGNAL);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            send(msg.getClient().getSocketNumber(), "Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                        break;
                    case 'w':
                        // Whois command
                        if (ss >> word)
                        {
                            for (Client *c : ch.getClients())
                            {
                                // Verificar se o cliente não é o mesmo que enviou a mensagem
                                if (word == c->getNickname())
                                {
                                    // Enviar a mensagem para o usuário
                                    string aux = "Ip de " + c->getNickname() + " é: " + c->getIp();
                                    send(msg.getClient().getSocketNumber(), aux.c_str(), MAX, MSG_NOSIGNAL);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            send(msg.getClient().getSocketNumber(), "Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                        break;
                    }
                }
                else
                {
                    // Não pode realizar os comandos
                    send(msg.getClient().getSocketNumber(), "Comando disponível apenas para o administrador do canal!\n", MAX, MSG_NOSIGNAL);
                }
            }
            else
            {
                // Checar se o usuário que mandou a mensagem não está mutado
                if (msg.getClient().getIsMuted())
                {
                    // Se estiver mutado apenas avisa que ele está mutado e não faz nada
                    send(msg.getClient().getSocketNumber(), "Você está mutado!\n", MAX, MSG_NOSIGNAL);
                }
                else
                {
                    for (Client *c : ch.getClients())
                    {
                        // Verificar se o cliente não é o mesmo que enviou a mensagem
                        if (msg.getClient().getNickname() == c->getNickname())
                            continue;

                        // Enviar a mensagem para o usuário
                        int socket = c->getSocketNumber();
                        string aux = msg.getClient().getNickname() + ": " + msg.getMessage();
                        send(socket, aux.c_str(), MAX, MSG_NOSIGNAL);
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