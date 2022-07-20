/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "clientManager.hpp"

#include <stdlib.h>
#include <bits/stdc++.h>
#include <string>
#include <vector>

using namespace std;

class Channel
{
    private:
        string name;
        string adminName;
        // Cada canal contém uma lista de usuários conectados a ele
        vector<Client *> clients;

    public:
        Channel(string name, Client *client)
        {
            // Cria o canal e já insere o primeiro usuário
            this->name = name;
            this->clients.push_back(client);
            this->adminName = client->getNickname();
        }

        vector<Client *> getClients()
        {
            return clients;
        }

        void insertClient(Client *c)
        {
            clients.push_back(c);
            cout << "Numero de clientes: " << clients.end() - clients.begin() << endl;
        }

        bool removeClient(string clientName)
        {
            int pos = 0;
            for(Client *it: clients)
            {
                // Verificar se o socket do cliente atual bate
                if(it->getNickname() == clientName)
                {
                    clients.erase(clients.begin()+pos);
                    cout << clientName << " removido com sucesso!" << endl;
                    cout << "Numero de clientes: " << clients.end() - clients.begin() << endl;
                    return true;
                }
                pos++;
            }

            return false;
        }

        string getAdminName()
        {
            return this->adminName;
        }
};

#endif