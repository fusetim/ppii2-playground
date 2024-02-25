#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef _LIB_SDL_NET_H
#define _LIB_SDL_NET_H
#define WITHOUT_SDL
#include <SDL2/SDL_net.h>
#endif

#include "../include/server.h"

#ifndef _LIB_HELPER_H
#define _LIB_HELPER_H
#include "../include/helper.h"
#endif

#ifndef _LIB_CMD_H
#define _LIB_CMD_H
#include "../include/cmd.h"
#endif

int main(int argc, char const *argv[])
{
    printf("Hello, World!\n");
    if (argc <= 1) {
        printf("Missing port number\n");
        return 1;
    }
    int port = atoi(argv[1]);
    printf("Launching server on port: %d\n", port);
    run_server(port);
    return 0;
}

void run_server(int port) {
    // Initialize SDLNet
    SDLNet_Init();
    printf("SDLNet initialized\n");

    // Open socket
    IPaddress ip;
    TCPsocket server;
    SDLNet_ResolveHost(&ip, NULL, port);
    server = SDLNet_TCP_Open(&ip);
    if (!server) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return;
    }

    ClientList clients;
    clients.active = 0;
    clients.set = SDLNet_AllocSocketSet(MAX_CLIENTS);
    int next_player_id = 0;

    printf("Running\n");
    bool running = true;
    while (running) {
        // Accept new clients if possible
        TCPsocket client = SDLNet_TCP_Accept(server);
        if (client) {
            if (clients.active < MAX_CLIENTS) {
                Player nplayer;
                nplayer.sock = client;
                nplayer.id = ++next_player_id;
                clients.player_table[clients.active] = nplayer;
                clients.active++;
                SDLNet_TCP_AddSocket(clients.set, client);
                IPaddress* ip = SDLNet_TCP_GetPeerAddress(client);
                char ipstr[24];
                addrtocstr(ip, ipstr);
                printf("new connection from: %s\n", ipstr);
            } else {
                printf("Max clients reached\n");
                SDLNet_TCP_Close(client);
            }
        }

        // Check for incoming data
        if(SDLNet_CheckSockets(clients.set, 0) > 0) {
            for (int i = 0; i < clients.active; i++) {
                TCPsocket client = clients.player_table[i].sock;
                if (SDLNet_SocketReady((SDLNet_GenericSocket) client)) {
                    on_recv(&clients, i);
                }
            }
        }
    }

    // Clean up
    SDLNet_TCP_Close(server);
    SDLNet_Quit();
}

void on_recv(ClientList* clients, int id) {
    uint8_t incoming[1024];
    uint8_t outgoing[1024];
    int outlen = 0;
    char cmd[5];
    TCPsocket client = clients->player_table[id].sock;
    IPaddress* ip = SDLNet_TCP_GetPeerAddress(client);
    char ipstr[24];
    addrtocstr(ip, ipstr);

    // If the message is not read correctly, kill the connection.
    if (!read_message(incoming, client)) {
        outlen = server_kick(outgoing, "invalid_command");
        SDLNet_TCP_Send(client, outgoing, outlen);
        outlen = server_quit(outgoing, clients->player_table[id].id);
        kill_connection(clients, id);
        send_to_all(clients, outgoing, outlen);
        return;
    }

    read_command(incoming, cmd);
    if (strcmp(cmd, "JOIN") == 0) {
        printf("Player%d (%s) joined.\n", clients->player_table[id].id, ipstr);
        outlen = server_acpt(outgoing, clients->player_table[id].id);
        SDLNet_TCP_Send(client, outgoing, outlen);
        outlen = server_join(outgoing, clients->player_table[id].id);
        send_to_all(clients, outgoing, outlen);
    } else if (strcmp(cmd, "QUIT") == 0) {
        printf("Player%d (%s) left.\n", clients->player_table[id].id, ipstr);
        outlen = server_byee(outgoing);
        SDLNet_TCP_Send(client, outgoing, outlen);
        outlen = server_quit(outgoing, clients->player_table[id].id);
        kill_connection(clients, id);
        send_to_all(clients, outgoing, outlen);
    } else if (strcmp(cmd, "MOVE") == 0) {
        uint16_t xpos, ypos;
        xpos = read_uint16(incoming + 6);
        ypos = read_uint16(incoming + 8);
        printf("Player%d (%s) move to (%d, %d).\n", clients->player_table[id].id, ipstr, xpos, ypos);
        outlen = server_move(outgoing, clients->player_table[id].id, xpos, ypos);
        send_to_all(clients, outgoing, outlen);
    } else {
        printf("Player%d (%s) sent unexpected command `%s` -> KICKED\n", clients->player_table[id].id, ipstr, cmd);
        outlen = server_kick(outgoing, "unknown_command");
        SDLNet_TCP_Send(client, outgoing, outlen);
        outlen = server_quit(outgoing, clients->player_table[id].id);
        kill_connection(clients, id);
        send_to_all(clients, outgoing, outlen);
    }
}

void kill_connection(ClientList* clients, int cid) {
    SDLNet_TCP_Close(clients->player_table[cid].sock);
    SDLNet_TCP_DelSocket(clients->set, clients->player_table[cid].sock);
    remove_client(clients, cid);
}

void send_to_all(ClientList* clients, uint8_t* msg, int len) {
    for (int i = 0; i < clients->active; i++) {
        TCPsocket client = clients->player_table[i].sock;
        SDLNet_TCP_Send(client, msg, len);
    }
}

void addrtocstr(IPaddress* ip, char str[24]) {
    uint8_t* addr = (uint8_t*)&ip->host;
    sprintf(str, "%d.%d.%d.%d:%d", addr[0], addr[1], addr[2], addr[3], ip->port);
}

void remove_client(ClientList* clients, int cid) {
    clients->active--;
    for (int i = cid; i < clients->active; i++) {
        clients->player_table[i] = clients->player_table[i + 1];
    }
}