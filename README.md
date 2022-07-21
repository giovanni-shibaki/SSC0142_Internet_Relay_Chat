# SSC0142_Internet_Relay_Chat
Projeto de desenvolvimento de um IRC em linguaguem C++ para a disciplina SSC0142 - Redes de Computadores

# Membros

- Giovanni Shibaki Camargo  Nusp: 11796444
- Lucas Keiti Anbo Mihara   Nusp: 11796472
- Vitor Caetano Brustolin   Nusp: 11795589

# Árvore de arquivos do projeto

    .
    ├── Link_para_o_GitHub.txt
    ├── Makefile
    ├── README.md
    ├── assets
    │   ├── mensagemEnorme.txt
    │   ├── video_chat.gif
    │   └── video_chat2.gif
    ├── client
    ├── lib
    │   ├── channel.hpp
    │   ├── channelManager.hpp
    │   ├── client.hpp
    │   ├── clientManager.hpp
    │   ├── message.hpp
    │   └── messageManager.hpp
    ├── server
    └── src
        ├── client.cpp
        └── server.cpp

# Como compilar e executar

## Compilar

Para compilar o projeto basta utilizar o comando **make** no arquivo Makefile fornecido, conforme o código abaixo:

> make

Para compilar foi utilizado o compilador **gcc** na versão **gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.1)**

## Executar

Após compilado, o projeto irá gerar dois arquivos executáveis: **client** e **server**, correspondentes ao programa de usuário e cliente que irão conversar na primeira entrega do projeto.

Para executar o cliente e o servidor, basta utilizar os comandos abaixo:

> make run_client

> make run_server

# Comandos

Abaixo, a lista de comandos para todos os usuários:

- Connect (/connect):
    - Usado para se conectar ao servidor logo no Início.
- Quit (/quit):
    - Usado para se desconectar do servidor
- Ping (/ping):
    - Quando usado o servidor irá lhe responder com uma mensagem "Pong"
- Join (/join \<nome do canal>):
    - Usado para entrar em algum dos canais, caso o canal em questão nao exista um novo canal será criado
- Nickname (/nickname \<apelido desejado>):
    - Usado para atribuir a si mesmo o apelido desejado
- Observacao:
    - Os nomes de canal e de nickname devem obedecer uma série de regras para serem considerados válidos.
    - Alem disso o servidor faz a checagem se há dois usuarios com o mesmo nome, portanto use um nome original.
    - Não será possível entrar em um canal caso ele esteja no modo privado e você não esteja na lista de convidados.

Abaixo, a lista de comandos para os administradores de canais (são aqueles que criam o canal):

- Kick (/kick \<nome do usuario>):
    - Esse comando expulsa o usuário em questão do canal.
- Mute (/mute \<nome do usuario>):
    - Esse comando faz com que o usuário em questão não consiga enviar mais mensagens no canal.
- Unmute (/unmute \<nome do usuario>):
    - Esse comando faz com que o usuário silenciado possa voltar a enviar mensagens normalmente.
- Whois (/whois \<nome do usuario>):
    - Esse comando retorna ao administrador do servidor o IP do usuário em questao.
- Mode (/mode):
    - Altera o modo do canal entre **privado** e **publico**.
- Invite (/invite \<nome do usuario>):
    - Convida o usuário em questão para o canal.

# Referências

- https://datatracker.ietf.org/doc/html/rfc1459
- https://www.rfc-editor.org/info/rfc1459
