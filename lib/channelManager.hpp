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

// changing Library according to OS
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#define sleep usleep
#endif

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
        if(it == channelMap.end())
        {
            // Não achou o canal
            return false;
        }
        return true;
    }

    void createChannel(string name, Client client)
    {
        Channel *newChannel = new Channel(name, client);
        channelMap.insert(pair<string, Channel>(name, *newChannel));
    }

    void insertClientChannel(string channelName, Client client)
    {
        channelMap.at(channelName).insertClient(client);
    }

    Channel getChannel(string channelName)
    {
        return channelMap.at(channelName);
    }
};

#endif