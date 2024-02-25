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
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return;
    }
    if (SDLNet_Init() != 0) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return;
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    if (init_window(&window, &renderer) != 0) {
        SDL_Event event;
        bool running = true;
        while (running) {
            
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect rect = {.x = 0, .y = 0, .w = 10, .h = 20};
            SDL_RenderDrawRect(renderer, &rect);
            SDL_RenderPresent(renderer);
        }
        //SDL_DestroyRenderer(renderer);
        //SDL_DestroyWindow(window);
    }

    // Close SDL&SDLNet
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