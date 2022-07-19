/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "client.hpp"

#include <stdlib.h>
#include <bits/stdc++.h>
#include <string>

using namespace std;

class Message
{
    private:
        Client client;
        string message;

    public:
        Message(Client c, string msg)
        {
            this->client = c;
            this->message = msg;
        }

        Client *getClient()
        {
            return &client;
        }

        string getMessage()
        {
            return message;
        }
};

#endif