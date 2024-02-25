#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#ifndef _LIB_CMD_H
#define _LIB_CMD_H
#include "../include/cmd.h"
#endif

#ifndef _LIB_HELPER_H
#define _LIB_HELPER_H
#include "../include/helper.h"
#endif

int server_join(uint8_t* msgbuf, uint8_t player_id) {
    char* command = "JOIN";
    uint16_t length = 1;
    return build_message(msgbuf, command, length, &player_id);
}

int server_quit(uint8_t* msgbuf, uint8_t player_id) {
    char* command = "QUIT";
    uint16_t length = 1;
    return build_message(msgbuf, command, length, &player_id);
}

int server_acpt(uint8_t* msgbuf, uint8_t player_id) {
    char* command = "ACPT";
    uint16_t length = 1;
    return build_message(msgbuf, command, length, &player_id);
}

int server_byee(uint8_t* msgbuf) {
    char* command = "BYEE";
    uint16_t length = 0;
    return build_message(msgbuf, command, length, NULL);
}

int server_move(uint8_t* msgbuf, uint8_t player_id, uint16_t xpos, uint16_t ypos) {
    char* command = "MOVE";
    uint16_t length = 5;
    uint8_t direction[5];
    write_uint16(direction, xpos);
    write_uint16(direction + 2, ypos);
    direction[4] = player_id;
    return build_message(msgbuf, command, length, direction);
}

int server_kick(uint8_t* msgbuf, char* reason) {
    char* command = "KICK";
    uint16_t length = strlen(reason) + 1;
    return build_message(msgbuf, command, length, (uint8_t*) reason);
}