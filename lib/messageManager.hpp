/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

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
        if (!messageQueue.empty())
        {
            mtx.lock();
            Message msg = messageQueue.front();
            messageQueue.pop();

            // Usar o ChannelManager para enviar mensagem para todos os clientes que estão na mesma sala do que enviou a mensagem
            // Passo a passo:
            // Encontrar o canal em que o cliente está conectado
            Channel ch = channelMan->getChannel(msg.getClient().getChannelName());
            cout << "Lista de clientes: " << endl;
            for(Client c: ch.getClients())
            {
                cout << c.getNickname() << endl;
            }

            // Pegar a lista de clientes conectados no mesmo canal
            // Enviar a mensagem para todos os usuários naquele canal menos para o usuário que a enviou
            

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
        cout << "Inseriu a mensagem: " << msg << endl;
        messageQueue.push(message);

        mtx.unlock();
        sleep(500);
    }
};

#endif