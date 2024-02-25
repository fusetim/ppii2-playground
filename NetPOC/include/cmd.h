#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int server_join(uint8_t *msgbuf, uint8_t player_id);
int server_quit(uint8_t *msgbuf, uint8_t player_id);
int server_acpt(uint8_t *msgbuf, uint8_t player_id);
int server_byee(uint8_t *msgbuf);
int server_move(uint8_t *msgbuf, uint8_t player_id, uint16_t xpos, uint16_t ypos);
int server_kick(uint8_t *msgbuf, char *reason);

int client_join(uint8_t *msgbuf);
int client_move(uint8_t *msgbuf, uint16_t xpos, uint16_t ypos);
int client_quit(uint8_t *msgbuf);