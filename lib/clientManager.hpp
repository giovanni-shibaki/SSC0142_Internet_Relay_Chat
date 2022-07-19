/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <bits/stdc++.h>
#include "client.hpp"

// changing Library according to OS
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#define sleep usleep
#endif

#include <vector>

using namespace std;

class ClientManager
{
private:
    vector<Client> clients;

public:
    ClientManager()
    {
    }

    Client insertClient(int socket, string ip, string nickname, string channelName)
    {
        Client c = Client(socket, ip, nickname, "");
        this->clients.push_back(c);
        return c;
    }

    bool checkUserName(string name)
    {
        // Checar se já existe um usuário com o nome enviado como argumento
        for (Client c : clients)
        {
            if (c.getNickname() == name)
            {
                return true;
            }
        }
        return false;
    }

    bool removeClient(string clientName)
    {
        int pos = 0;
        for (Client it : clients)
        {
            // Verificar se o socket do cliente atual bate
            if (it.getNickname() == clientName)
            {
                clients.erase(clients.begin() + pos);
                cout << clientName << " removido com sucesso da lista de clientes!" << endl;
                return true;
            }
            pos++;
        }

        return false;
    }
};

#endif