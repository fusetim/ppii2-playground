CC = gcc
CC_FLAGS = -fsanitize=address -Wall -Wextra -std=c99 -g
CC_LIBS = -lpthread -lSDL2 -lSDL2_net -lm $(shell sdl2-config --cflags --libs)

all: client server

debug_server: server
	valgrind ./build/server 9999

run_server: server
	./build/server 9999

client: before_build src/client.c
	$(CC) $(CC_FLAGS) $(CC_LIBS) -o ./build/client src/client.c

server: before_build src/server.c
	$(CC) $(CC_FLAGS) $(CC_LIBS) -o ./build/server src/server.c

before_build:
	mkdir -p build
	mkdir -p build/deps