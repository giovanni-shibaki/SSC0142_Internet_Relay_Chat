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
queue<Message *> messageQueue;
bool isServerActive = true;

static void *sendMessage(void *arg)
{
    while (isServerActive)
    {
        if (!messageQueue.empty())
        {
            Message *msg = messageQueue.front();
            messageQueue.pop();

            sem_wait(&notEmpty); // sem_wait -> decrease value
            mtx.lock();

            curEnergy--; // remove_item()

            mtx.unlock();
            sem_post(&notFull);  // sem_post increase value
            gt->receiveEnergy(); // consume_item()
            sleep(500000);
        }
    }
}

class MessageManager
{
private:
    // Produtor irá receber as novas mensagens enviadas pelos clientes
    // Consumidor irá enviar a mensagem para todos os clientes que estão no mesmo chat do cliente que enviou
    pthread_t producer, consumer;

public:
    void start()
    {
        pthread_attr_t attr;

        sem_init(&notEmpty, 0, 0);
        sem_init(&notFull, 0, 100);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        int r;

        r = pthread_create(&this->producer, &attr, generateEnergy, NULL);
        if (r)
        {
            cout << "Error in creating thread" << endl;
            exit(-1);
        }

        r = pthread_create(&this->consumer, &attr, consumeEnergy, NULL);
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
};

#endif