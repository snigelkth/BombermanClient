//
// Created by Timothy Friberg Holmsten on 19/04/16.
//
#include <SDL_events.h>
#include "player.h"
#include "../game/game.h"

void create_player(Dlist *list,int *playerCount, int x, int y, int id)
{
    printf("%d\n", *playerCount);
    dlist_insert_last(list, dlist_createElement(id,x,y));
    *playerCount+=1;
}

void update_players(Dlist * players) {

}

void player_place_bomb(DlistElement * player, Game *game)
{
    for(int bomb = 0; bomb < GAME_MAX_BOMBS;bomb++) {
        if (player->bombs[bomb].placed != 1) {
            player->bombs[bomb] = create_bomb(player->x, player->y, 1, player->id, 3);
            player->bombs[bomb].order = GAME_MAX_BOMBS-1-bomb;
            player->bombs_count += 1;
            player->bombs[bomb].placed = 1;
            char msg[100]; // Send this to connected device
            sprintf(msg, "4 %d %d %d \n", get_list_postition(&game->players, 0)->id, get_list_postition(&game->players, 0)->x, get_list_postition(&game->players, 0)->y);
            client_send(game, msg);
            break;
        }
    }
}

void update_local_player(DlistElement * player, Map * map, Game *game) {

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    int x = 0;
    int y = 0;

    if (state[SDL_SCANCODE_A])
        x = -1;
    if (state[SDL_SCANCODE_D])
        x =  1;
    if (state[SDL_SCANCODE_S])
        y =  1;
    if (state[SDL_SCANCODE_W])
        y = -1;
    if (state[SDL_SCANCODE_SPACE])
        player_place_bomb(player, game);

    if(player!= NULL) {
        if (!map_is_blocked(map, player->x + x, player->y) && x != 0) {
            player->x += x;
        }

        if (!map_is_blocked(map, player->x, player->y + y) && y != 0) {
            player->y += y;
        }
        if (x != 0 || y != 0)
            SDL_Delay(100);
    }
    //SDL_Delay(200);
}





