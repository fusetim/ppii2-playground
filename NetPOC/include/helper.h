#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef _LIB_SDL_NET_H
#define _LIB_SDL_NET_H
#define WITHOUT_SDL
#include <SDL2/SDL_net.h>
#endif

void write_uint16(uint8_t* buf, uint16_t value);
uint16_t read_uint16(uint8_t* buf);
void write_uint32(uint8_t* buf, uint32_t value);
uint32_t read_uint32(uint8_t* buf);

// Build a message from its components (command, length, body)
// Returns the length of the message
//
// msgbuf: buffer to store the message (it must be at least length + 7 bytes long)
// command: command to be sent
// length: length of the body
// body: body of the message
int build_message(uint8_t* msgbuf, char* command, uint16_t length, uint8_t* body);

// Read a message from a socket
// Returns true if the message was read successfully
// NOTE: buf is expected to be large. Let's assume it is at least 1024 bytes long.
bool read_message(uint8_t *buf, TCPsocket client);

// Read a command from a buffer
// cmd: buffer to store the command (must be at least 5 bytes long)
// buf: buffer containing the message (must be at least 4 bytes long)
void read_command(uint8_t *buf, char *cmd);