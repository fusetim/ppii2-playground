#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define WITHOUT_SDL
#include <SDL2/SDL_net.h>
#include "server.h"

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
                printf("Client connected\n");
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
    uint8_t buf[6];
    uint8_t zero = 0;
    char *cmd_str = (char*)buf;
    TCPsocket client = clients->player_table[id].sock;
    IPaddress* address = SDLNet_TCP_GetPeerAddress(client);
    int received = SDLNet_TCP_Recv(client, buf, 6);
    char addr_str[24];
    addrtocstr(address, addr_str);
    if (received < 0) {
        printf("Error receiving data: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(client);
        remove_client(clients, id);
        send_quit(clients, id);
    } else if (received >= 0) {
        printf("Received: %s from %s\n", cmd_str, addr_str);
        if (strcmp(cmd_str, "JOIN") == 0) {
            int received = SDLNet_TCP_Recv(client, buf, 1);
            if (received >= 1) {
                printf("%s sent JOIN\n", addr_str);
                uint8_t join_ack[] = {'A', 'C', 'P', 'T', 0, 1, (uint8_t) clients->player_table[id].id, '\n'};
                SDLNet_TCP_Send(client, join_ack, 8);
                uint8_t join_msg[] = {'J', 'O', 'I', 'N', 0, 1, (uint8_t) clients->player_table[id].id, '\n'};
                send_to_all(clients, join_msg, 8);
            } else {
                printf("Received invalid command: missing newline\n");
                kick_invalid_cmd(clients, id);
                kill_connection(clients, id);
                send_quit(clients, id);
            }
        } else if (strcmp(cmd_str, "QUIT") == 0) {
            int received = SDLNet_TCP_Recv(client, buf, 1);
            if (received >= 1) {
                printf("%s sent QUIT\n", addr_str);
                send_byee(clients, id);
                kill_connection(clients, id);
                send_quit(clients, id);
            } else {
                printf("Received invalid command: missing newline\n");
                kick_invalid_cmd(clients, id);
                kill_connection(clients, id);
                send_quit(clients, id);
            }
        }
    } else {
        printf("Received invalid command\n");
        kick_invalid_cmd(clients, id);
        kill_connection(clients, id);
        send_quit(clients, id);
    }
}

void kill_connection(ClientList* clients, int cid) {
    SDLNet_TCP_Close(clients->player_table[cid].sock);
    SDLNet_TCP_DelSocket(clients->set, clients->player_table[cid].sock);
    remove_client(clients, cid);
}

void kick_invalid_cmd(ClientList* clients, int cid) {
    TCPsocket client = clients->player_table[cid].sock;
    uint8_t kick_msg[] = {'K', 'I', 'C', 'K', 0, 8, 'I', 'N', 'V', '_', 'C', 'M', 'D', '\0', '\n'};
    SDLNet_TCP_Send(client, kick_msg, 15);
}

void send_quit(ClientList* clients, int cid) {
    uint8_t quit_msg[] = {'Q', 'U', 'I', 'T', 0, 4, (uint8_t) clients->player_table[cid].id, '\n'};
    send_to_all(clients, quit_msg, 8);
}

void send_to_all(ClientList* clients, uint8_t* msg, int len) {
    for (int i = 0; i < clients->active; i++) {
        TCPsocket client = clients->player_table[i].sock;
        SDLNet_TCP_Send(client, msg, len);
    }
}

void send_byee(ClientList* clients, int cid) {
    uint8_t byee_msg[] = {'B', 'Y', 'E', 'E', 0, 0, '\n'};
    SDLNet_TCP_Send(clients->player_table[cid].sock, byee_msg, 7);
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