#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#ifndef _LIB_HELPER_H
#define _LIB_HELPER_H
#include "../include/helper.h"
#endif

void write_uint16(uint8_t* buf, uint16_t value) {
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
}

uint16_t read_uint16(uint8_t* buf) {
    return (buf[0] << 8) | buf[1];
}

void write_uint32(uint8_t* buf, uint32_t value) {
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
}

uint32_t read_uint32(uint8_t* buf) {
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

int build_message(uint8_t* msgbuf, char* command, uint16_t length, uint8_t* body) {
    memcpy(msgbuf, (uint8_t*) command, 4);
    write_uint16(msgbuf + 4, length);
    if (length > 0)
        memcpy(msgbuf + 6, body, length);
    msgbuf[6 + length] = '\n';
    return 7 + length;
}

// buf is expected to be large. Let's assume it is 1024 bytes long.
bool read_message(uint8_t* buf, TCPsocket sock) {
    int received = SDLNet_TCP_Recv(sock, buf, 6);
    if (received < 0) {
        printf("Error receiving data: %s\n", SDLNet_GetError());
        return false;
    }
    if (received < 6) {
        printf("Received invalid command: missing newline\n");
        return false;
    }
    uint16_t length = read_uint16(buf + 4);
    if (length > 512) {
        printf("Received invalid message: too long, length is %d\n", length);
        return false;
    }
    received = SDLNet_TCP_Recv(sock, buf + 6, length+1);
    if (received < 0) {
        printf("Error receiving data: %s\n", SDLNet_GetError());
        return false;
    }
    if (received < length+1) {
        printf("Received invalid message: truncated.\n");
        return false;
    }
    return true;
}

// len(buf) >= 4
// len(cmd) >= 5
void read_command(uint8_t* buf, char* cmd) {
    memcpy(cmd, buf, 4);
    cmd[4] = '\0';
}