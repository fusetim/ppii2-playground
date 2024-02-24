# Protocol

The protocol is formatted as :
```
COMMAND LEN ARG1 ARG2...\n
```

NOTE: All data is written in network endian / big endian.

NOTE: All commands are 4 char-long.

NOTE: LEN is alaways an uint16.

## CLIENT COMMANDS

All client commands are 4 char-long.

Available client commands are:

* `JOIN` -> Take 0 argument, len is 0.
* `QUIT` -> Take 0 argument, len is 0.
* `MOVE` -> Take 2 arguments, len is 4.

### JOIN

**Description:** Registers a client to the server and to the other players.

**Args:** None

**Length:** MUST BE 0.

### QUIT

**Description:** Quits the server and informs the other players.

**Args:** None

**Length:** MUST BE 0.

### MOVE

**Description:** Indicate the new coordinate of the player.

**Args:**
* X position (uint16)
* Y position (uint16)

**Length:** MUST BE 4.

## SERVER COMMANDS

All client commands are 4 char-long.

Available client commands are:

* `JOIN` -> Take 1 argument, len is 1.
* `QUIT` -> Take 1 argument, len is 1.
* `MOVE` -> Take 3 arguments, len is 5.
* `KICK` -> Take 1 argument, len is variable.
* `BYEE` -> Take 0 argument, len is 0.
* `ACPT` -> Take 0 argument, len is 0.

### JOIN

**Description:** A player joined the party.

**Args:**
* Player ID (uint8)

**Length:** MUST BE 1.

### QUIT

**Description:** A player left the server.

**Args:**
* Player ID (uint8)

**Length:** MUST BE 1.

### MOVE

**Description:** Indicate the new coordinate of a player.

**Args:**
* X position (uint16)
* Y position (uint16)
* Player ID (uint8)

**Length:** MUST BE 8.

### KICK

**Description:** Kicked from server.

**Args:**
* Reason (NUL-terminated C-string)

**Length:** length of reason.

### BYEE

**Description:** Acknowledges the left of the client.

**Args:** None

**Length:** MUST BE 0.

### ACPT

**Description:** Acknowledges the join of the player.

**Args:**
* Player ID of the current player, as uint8

**Length:** MUST BE 1.