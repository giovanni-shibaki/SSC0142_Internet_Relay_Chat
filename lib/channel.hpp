/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef CHANNEL_H
#define CHANNEL_H

// changing Library according to OS
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#define sleep usleep
#endif

#include <stdlib.h>
#include <bits/stdc++.h>
#include <string>

using namespace std;

class Channel
{
    private:
        int number;
        string name;


    public:
        Channel(Client c, string msg)
        {
            this->client = c;
            this->message = msg;
        }

        Client getClient()
        {
            return client;
        }

        string getMessage()
        {
            return message;
        }
};

#endif