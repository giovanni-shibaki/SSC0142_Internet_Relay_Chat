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

static void *messageConsumer(void *arg)
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
                    if ("/kick" == word)
                    {
                        // Kick command
                        if (ss >> word)
                        {
                            bool flag = false;
                            for (Client *c : ch.getClients())
                            {
                                if (word == c->getNickname())
                                {
                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    send(socket, ">> Você foi desconectado do servidor!\n", MAX, MSG_NOSIGNAL);

                                    // Remover o usuário da sala
                                    c->setIsActive(false);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                send(msg.getClient()->getSocketNumber(), ">> Usuário não encontrado!\n", MAX, MSG_NOSIGNAL);
                            }
                        }
                        else
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                    }
                    else if ("/mute" == word)
                    {
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
                                        send(msg.getClient()->getSocketNumber(), ">> O usuário já se encontra mutado!\n", MAX, MSG_NOSIGNAL);
                                        flag = true;
                                        break;
                                    }

                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    c->mute();
                                    send(socket, ">> Você foi mutado pelo administrador do canal!\n", MAX, MSG_NOSIGNAL);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                send(msg.getClient()->getSocketNumber(), ">> Usuário não encontrado!\n", MAX, MSG_NOSIGNAL);
                            }
                        }
                        else
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                    }
                    else if ("/unmute" == word)
                    {
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
                                        send(msg.getClient()->getSocketNumber(), ">> O usuário já se encontra desmutado!\n", MAX, MSG_NOSIGNAL);
                                        flag = true;
                                        break;
                                    }

                                    // Enviar a mensagem para o usuário
                                    int socket = c->getSocketNumber();
                                    c->unmute();
                                    send(socket, ">> Você foi desmutado pelo administrador do canal!\n", MAX, MSG_NOSIGNAL);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                send(msg.getClient()->getSocketNumber(), ">> Usuário não encontrado!\n", MAX, MSG_NOSIGNAL);
                            }
                        }
                        else
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                    }
                    else if ("/whois" == word)
                    {
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
                                    send(msg.getClient()->getSocketNumber(), aux.c_str(), MAX, MSG_NOSIGNAL);
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag)
                            {
                                send(msg.getClient()->getSocketNumber(), ">> Usuário não encontrado!\n", MAX, MSG_NOSIGNAL);
                            }
                        }
                        else
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                    }
                    else if ("/invite" == word)
                    {
                        // Invite Command
                        if (ss >> word)
                        {
                            channelMan->inviteClient(msg.getClient()->getChannelName(), word);
                            string aux = ">> " + word + " foi convidado para o canal\n";
                            send(msg.getClient()->getSocketNumber(), aux.c_str(), MAX, MSG_NOSIGNAL);
                        }
                        else
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Faltou algum argumento, comando inválido!\n", MAX, MSG_NOSIGNAL);
                        }
                    }
                    else if ("/mode" == word)
                    {
                        // Mode Command
                        if (channelMan->changeMode(msg.getClient()->getChannelName()))
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Canal trocado para o modo privado\n", MAX, MSG_NOSIGNAL);
                        }
                        else
                        {
                            send(msg.getClient()->getSocketNumber(), ">> Canal trocado para o modo publico\n", MAX, MSG_NOSIGNAL);    
                        }
                    }
                    else if("/ping" == word)
                    {
                        // Ping Command
                        // É feito de forma automática na função receiveMessage() do server.cpp
                    }
                    else
                    {
                        // Nenhum comando encontrado
                        send(msg.getClient()->getSocketNumber(), ">> Comando inválido!\n", MAX, MSG_NOSIGNAL);
                    }
                }
                else
                {
                    // Não pode realizar os comandos caso não seja administrador
                    send(msg.getClient()->getSocketNumber(), ">> Comando disponível apenas para o administrador do canal!\n", MAX, MSG_NOSIGNAL);
                }
            }
            else
            {
                // Checar se o usuário que mandou a mensagem não está mutado
                if (msg.getClient()->getIsMuted())
                {
                    // Se estiver mutado apenas avisa que ele está mutado e não faz nada
                    send(msg.getClient()->getSocketNumber(), ">> Você está mutado!\n", MAX, MSG_NOSIGNAL);
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
                        send(socket, aux.c_str(), MAX, MSG_NOSIGNAL);
                    }
                }
            }
            mtx.unlock();
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

        r = pthread_create(&this->consumer, &attr, messageConsumer, NULL);
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
        // Checar se o cliente está ativo
        if (!c.getIsActive())
            return;

        mtx.lock();

        // Inserir a mensagem na Fila de mensagens
        Message message = Message(c, msg);
        messageQueue.push(message);

        mtx.unlock();
    }
};

#endif