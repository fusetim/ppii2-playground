#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#ifndef _LIB_SDL_NET_H
#define _LIB_SDL_NET_H
#define WITHOUT_SDL
#include <SDL2/SDL_net.h>
#endif

#define WIDTH 600
#define HEIGHT 400

int init_window(SDL_Window** window, SDL_Renderer** renderer);
void run_client(int port);