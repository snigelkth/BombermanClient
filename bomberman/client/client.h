//
// Created by martin on 4/21/16.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H
#include <SDL.h>
#include <SDL_net.h>
#include <stdbool.h>
#include "../main.h"
#include "../player/player.h"
#include "../object/objects/bomb/bomb.h"


int initClient(Game *game);
void client_recv( Game *game);
void client_send(Game *game, char *msg);
void client_EXIT(TCPsocket client);
#endif //CLIENT_CLIENT_H
