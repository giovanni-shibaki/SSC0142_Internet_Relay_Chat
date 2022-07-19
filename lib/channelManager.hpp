/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <bits/stdc++.h>
#include "channel.hpp"
#include "utils.hpp"

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

    void insertClientChannel(string channelName, Client *client)
    {
        channelMap.at(channelName).insertClient(client);

        // Mandar mensagem para todos os clientes conectados
        for (Client *c : channelMap.at(channelName).getClients())
        {
            sendMessage(c->getSocketNumber(), string(client->getNickname() + " entrou no canal!\n"), MAX, &channelMap.at(channelName), c);
        }
    }

    Channel getChannel(string channelName)
    {
        return channelMap.at(channelName);
    }

    bool kickClient(string channelName, string clientName)
    {
        if (channelMap.at(channelName).removeClient(clientName))
        {
            // Usuário removido do canal
            // Mandar mensagem para todos os clientes conectados
            for (Client *c : channelMap.at(channelName).getClients())
            {
                sendMessage(c->getSocketNumber(), string(clientName + " saiu no canal!\n"), MAX, &channelMap.at(channelName), c);
            }
            return true;
        }
        return false;
    }
};

#endif