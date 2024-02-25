CC = gcc
CC_FLAGS = -fsanitize=address -Wall -Wextra -std=c99 -g
CC_LIBS = -lpthread -lSDL2 -lSDL2_net -lm $(shell sdl2-config --cflags --libs)

all: client server

debug_server: server
	valgrind ./build/server 9999

run_server: server
	./build/server 9999

run_client: client
	./build/client 9999

client: before_build lib src/client.c
	$(CC) $(CC_FLAGS) $(CC_LIBS) -o ./build/client src/client.c ./build/deps/lib.a

server: before_build lib src/server.c
	$(CC) $(CC_FLAGS) $(CC_LIBS) -o ./build/server src/server.c ./build/deps/lib.a

lib: before_build src/cmd.c src/helper.c
	$(CC) $(CC_FLAGS) -c src/cmd.c -o ./build/deps/cmd.o
	$(CC) $(CC_FLAGS) -c src/helper.c -o ./build/deps/helper.o
	ar rcs ./build/deps/lib.a ./build/deps/cmd.o ./build/deps/helper.o

before_build:
	mkdir -p build
	mkdir -p build/deps