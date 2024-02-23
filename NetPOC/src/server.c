#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <SDL2/SDL.h>
#define WITHOUT_SDL
#include <SDL2/SDL_net.h>

int main(int argc, char const *argv[])
{
    printf("Hello, World!\n");
    //SDL_Init(SDL_INIT_EVERYTHING);
    if (argc <= 1) {
        printf("Missing port number\n");
        return 1;
    }
    int port = atoi(argv[1]);
    printf("Launching server on port: %d\n", port);
    SDLNet_Init();
    printf("SDLNet initialized\n");
    IPaddress ip;
    TCPsocket server;
    SDLNet_ResolveHost(&ip, NULL, port);
    printf("Host resolved\n");
    server = SDLNet_TCP_Open(&ip);
    if (!server) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 2;
    }
    printf("Running\n");
    bool running = true;
    while (running) {
        TCPsocket client = SDLNet_TCP_Accept(server);
        if (client) {
            char text[100];
            int received = SDLNet_TCP_Recv(client, text, 100);
            if (received > 0) {
                printf("Received: %s\n", text);
                SDLNet_TCP_Send(client, text, received);
            }
            SDLNet_TCP_Close(client);
        }
    }
    SDLNet_TCP_Close(server);
    SDLNet_Quit();
    //SDL_Quit();
    return 0;
}
