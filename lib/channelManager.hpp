/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <bits/stdc++.h>
#include <sys/socket.h>
#include "channel.hpp"

#define MAX 4096

using namespace std;

class ChannelManager
{
private:
    map<string, Channel> channelMap;
    bool isServerActive = true;
    int numChannels = 0;

public:
    ChannelManager()
    {
    }

    bool isChannelActive(string name)
    {
        map<string, Channel>::iterator it;
        it = channelMap.find(name);
        if (it == channelMap.end())
        {
            // Não achou o canal
            return false;
        }
        return true;
    }

    void createChannel(string name, Client *client)
    {
        Channel *newChannel = new Channel(name, client);
        channelMap.insert(pair<string, Channel>(name, *newChannel));
    }

    bool insertClientChannel(string channelName, Client *client)
    {
        if (channelMap.at(channelName).isClientAllowedToEnter(client->getNickname()))
        {
            channelMap.at(channelName).insertClient(client);
        }
        else
        {
            return false;
        }

        // Mandar mensagem para todos os clientes conectados
        for (Client *c : channelMap.at(channelName).getClients())
        {
            string aux = client->getNickname() + " entrou no canal!\n";
            send(c->getSocketNumber(), aux.c_str(), MAX, MSG_NOSIGNAL);
        }
        return true;
    }

    Channel getChannel(string channelName)
    {
        return channelMap.at(channelName);
    }

    bool kickClient(string channelName, string clientName)
    {
        try
        {
            Channel ch = channelMap.at(channelName);
            if (channelMap.at(channelName).removeClient(clientName))
            {
                // Usuário removido do canal
                // Mandar mensagem para todos os clientes conectados
                for (Client *c : ch.getClients())
                {
                    string aux = clientName + " saiu no canal!\n";
                    send(c->getSocketNumber(), aux.c_str(), MAX, MSG_NOSIGNAL);
                }
                return true;
            }
            return false;
        }
        catch (const out_of_range& err)
        {
            cout << "Erro ao remover usuário" << endl;
            return false;
        }
    }

    bool changeMode(string channelName) 
    {
        return channelMap.at(channelName).changeMode();
    }

    void inviteClient(string channelName, string nick)
    {
        channelMap.at(channelName).inviteClient(nick);
    }
};

#endif