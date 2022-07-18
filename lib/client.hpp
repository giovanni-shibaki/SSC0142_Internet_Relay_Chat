/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef CLIENT_H
#define CLIENT_H

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

class Client
{
    private:
        int socketNumber;
        string ip;
        string nickname;
        string channelName;
        bool isMuted;
        bool isActive;

    public:
        Client()
        {
            
        }

        Client(int socket, string ip, string nickname, string channelName)
        {
            this->socketNumber = socket;
            this->ip = ip;
            this->nickname = nickname;
            this->channelName = channelName;
            this->isMuted = false;
            this->isActive = true;
        }

        string getIp()
        {
            return ip;
        }

        string getNickname()
        {
            return nickname;
        }

        void setNickname(string nick)
        {
            this->nickname = nick;
        }

        string getChannelName()
        {
            return channelName;
        }

        void setChannelName(string name)
        {
            this->channelName = name;
        }

        int getSocketNumber()
        {
            return socketNumber;
        }

        bool getIsMuted()
        {
            return isMuted;
        }

        void mute()
        {
            this->isMuted = true;
        }

        void unmute()
        {   
            this->isMuted = false;
        }

        void kick()
        {
            close(socketNumber);
        }

        bool getIsActive()
        {
            return isActive;
        }

        void setIsActive(bool flag)
        {
            this->isActive = flag;
        }
};

#endif