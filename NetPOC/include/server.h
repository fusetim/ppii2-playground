#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define WITHOUT_SDL
#include <SDL2/SDL_net.h>

#define MAX_CLIENTS 4

typedef struct {
    TCPsocket sock;
    int id;
} Player;

typedef struct {
    // Number of active clients
    int active;
    // Array of clients
    Player player_table[MAX_CLIENTS];
    // Socket set
    SDLNet_SocketSet set;
} ClientList;

void run_server(int port);

void on_recv(ClientList* clients, int id);

void kill_connection(ClientList* clients, int cid);

void send_to_all(ClientList* clients, uint8_t* msg, int len);

void remove_client(ClientList* clients, int cid);

void addrtocstr(IPaddress* ip, char str[24]);
