#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/client.h"

#include <SDL2/SDL.h>

#ifndef _LIB_SDL_NET_H
#define _LIB_SDL_NET_H
#include <SDL2/SDL_net.h>
#endif

#ifndef _LIB_HELPER_H
#define _LIB_HELPER_H
#include "../include/helper.h"
#endif

#ifndef _LIB_CMD_H
#define _LIB_CMD_H
#include "../include/cmd.h"
#endif

typedef struct {
    int id;
    int xpos;
    int ypos;
} PlayerInfo;

int main(int argc, char const *argv[])
{
    printf("NETPoC-Client HELO!\n");
    if (argc <= 1) {
        printf("Missing port number\n");
        return 1;
    }
    int port = atoi(argv[1]);
    run_client(port);
    return 0;
}

void run_client(int port) {
    // Init SDL&SDLNet
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return;
    }
    if (SDLNet_Init() != 0) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return;
    }

    // Open socket
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);
    IPaddress ip;
    TCPsocket ssock;
    SDLNet_ResolveHost(&ip, "localhost", port);
    ssock = SDLNet_TCP_Open(&ip);
    if (!ssock) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return;
    }
    SDLNet_TCP_AddSocket(set, ssock);

    // Send JOIN message
    uint8_t incoming[1024];
    uint8_t outgoing[1024];
    int len = client_join(outgoing);
    if (SDLNet_TCP_Send(ssock, outgoing, len) < len) {
        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return;
    }

    // Launch game loop
    SDL_Window* window;
    SDL_Renderer* renderer;
    PlayerInfo mePI = {.id = -1, .xpos = 0, .ypos = 0};
    PlayerInfo othersPI[3];
    for (int i = 0; i < 3; i++) {
        othersPI[i].id = -1;
    }
    if (init_window(&window, &renderer) == 0) {
        SDL_Event event;
        bool running = true;
        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            mePI.ypos -= 10;
                            break;
                        case SDLK_DOWN:
                            mePI.ypos += 10;
                            break;
                        case SDLK_LEFT:
                            mePI.xpos -= 10;
                            break;
                        case SDLK_RIGHT:
                            mePI.xpos += 10;
                            break;
                        default:
                            break;
                    }
                    len = client_move(outgoing, mePI.xpos, mePI.ypos);
                    if (SDLNet_TCP_Send(ssock, outgoing, len) < len) {
                        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
                    }
                }
            }
            SDLNet_CheckSockets(set, 0);
            if (SDLNet_SocketReady(ssock)) {
                if (!read_message(incoming, ssock)) {
                    printf("Error reading message\n");
                    running = false;
                    break;
                }
                char command[5];
                read_command(incoming, command);
                if (strcmp(command, "ACPT") == 0) {
                    uint8_t id = incoming[6];
                    mePI.id = id;
                    printf("Joined with id: %d\n", id);
                } else if (strcmp(command, "MOVE") == 0) {
                    uint16_t xpos = read_uint16(incoming + 6);
                    uint16_t ypos = read_uint16(incoming + 8);
                    uint8_t id = incoming[10];
                    if (id == mePI.id) {
                        mePI.xpos = xpos;
                        mePI.ypos = ypos;
                    } else {
                        for (int i = 0; i < 3; i++) {
                            if (othersPI[i].id == id) {
                                othersPI[i].xpos = xpos;
                                othersPI[i].ypos = ypos;
                                break;
                            }
                        }
                    }
                } else if (strcmp(command, "BYEE") == 0) {
                    printf("Server closed the connection\n");
                    running = false;
                    break;
                } else if (strcmp(command, "KICK") == 0) {
                    printf("Kicked by the server, reason: %s\n", incoming + 6);
                    running = false;
                    break;
                } else if (strcmp(command, "QUIT") == 0) {
                    uint8_t id = incoming[6];
                    printf("Player%d left\n", id);
                    for (int i = 0; i < 3; i++) {
                        if (othersPI[i].id == id) {
                            othersPI[i].id = -1;
                            break;
                        }
                    }
                } else if (strcmp(command, "JOIN") == 0) {
                    uint8_t id = incoming[6];
                    if (id == mePI.id) {
                        continue;
                    }
                    printf("Player%d joined\n", id);
                    for (int i = 0; i < 3; i++) {
                        if (othersPI[i].id == -1) {
                            othersPI[i].id = id;
                            break;
                        }
                    }
                } else {
                    printf("Received invalid command: %s\n", command);
                }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect rect = {mePI.xpos, mePI.ypos, 10, 10};
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255);
            for (int i = 0; i < 3; i++) {
                if (othersPI[i].id != -1) {
                    SDL_Rect rect = {othersPI[i].xpos, othersPI[i].ypos, 10, 10};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
            SDL_RenderPresent(renderer);
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    // Disconnect
    len = client_quit(outgoing);
    if (SDLNet_TCP_Send(ssock, outgoing, len) < len) {
        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    }

    // Close SDL&SDLNet
    SDLNet_TCP_Close(ssock);
    SDLNet_FreeSocketSet(set);
    SDLNet_Quit();
    SDL_Quit();
}

int init_window(SDL_Window** window, SDL_Renderer** renderer) {
    *window = SDL_CreateWindow("NETPoC-Client", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("SDL_CreateWindow: %s\n", SDL_GetError());
        return -1;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("SDL_CreateRenderer: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}