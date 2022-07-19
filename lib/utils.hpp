/*
    Internet Chat Relay
    Giovanni Shibaki Camargo        11796444
    Lucas Keiti Anbo Mihara         11796472
    Vitor Caetano Brustolin         11795589
*/

#ifndef UTILS_H
#define UTILS_H

// changing Library according to OS
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#define sleep usleep
#endif

#include "channel.hpp"
#include "client.hpp"
#include "message.hpp"

#include <sys/socket.h>

#define MAX 4096

using namespace std;

/**
 * @brief Função responsável pelo envio de mensagens por parte do servidor
 * Checa se a mensagem foi enviada com sucesso, caso contrário tenta reenviar até 5 vezes
 * Caso não consiga enviar nessas 5x, encerra a conexão com o cliente
 * 
 * @param socket 
 * @param msg 
 * @param size 
 * @param ch 
 * @param client
 * @return bool
 */
bool sendMessage(int socket, string msg, int size, Channel *ch, Client *client)
{
    bool flag = false;
    for(int i=0; i<5; i++)
    {
        send(socket, msg.c_str(), size, MSG_NOSIGNAL);

        char ret[MAX];
        if(read(socket, ret, MAX) == -1)
            break;
        string word;
        istringstream ss(ret);
        ss >> word;
        if(strcmp("<msgConfirm>", word.c_str()) == 0)
        {
            flag = true;
            break;
        }
    }
    if(!flag)
    {
        if(ch == NULL)
            return false;
        // Falha ao enviar mensagem, desconectar o cliente
        // Remover o usuário da sala
        cout << "Falha ao enviar mensagem, desconectando cliente: " << client->getNickname() << endl;
        ch->removeClient(client->getNickname());
        close(client->getSocketNumber());
        client->setIsActive(false);
        return false;
    }
    return true;
}

#endif