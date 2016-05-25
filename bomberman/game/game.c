//
// Created by Timothy Friberg Holmsten on 19/04/16.
//

#include <SDL_ttf.h>
#include "game.h"
#include "renderer/player/renderPlayer.h"
#include "renderer/object/renderObject.h"



struct local_player_args{
    Map * map;
    DlistElement * player;
    Game * game;
};


void * thread_update_player(void * arg) {

    struct local_player_args *arguments = (struct local_player_args*) arg;
    int i = 1;
    Uint32 playerUpdate = 0;
    while (i) {
        if (get_list_postition(&arguments->game->players, 0) != NULL && get_list_postition(&arguments->game->players, 0)->alive == 1){
            update_local_player(arguments->player, arguments->map, arguments->game, &playerUpdate);
        }
        SDL_Delay(16);
    }

    return NULL;
}
void * thread_multiplayer(void * arg) {
    //Multiplayer
    struct local_player_args *arguments = (struct local_player_args*) arg;
    int i = 1;
    while (i) {
        client_recv(arguments->game);
        SDL_Delay(1);
    }

    return NULL;
}

void * thread_update_bombs(void * arg) {

    struct local_player_args *arguments = (struct local_player_args*) arg;
    int i = 1;
    while (i) {
        if (get_list_postition(&arguments->game->players, 0) != NULL ) {
            for (int k = 0; k < dlist_size(&arguments->game->players); k++) {
                update_bombs(get_list_postition(&arguments->game->players, k)->bombs, arguments->map);
            }
        }
        SDL_Delay(16);
    }

    return NULL;
}

int init_game(SDL_Window *window, SDL_Renderer *renderer, Game * game) {
    initClient(game);
    pthread_t t1, t2;

    //Arguments for update thread
    /*struct args data;
    data.walls = game->walls;
    data.players = &game->players;

    pthread_create(&t1, NULL,init_update, &data );*/

    struct local_player_args local_p_data;
    local_p_data.map = &game->map;
    local_p_data.player = get_list_postition(&game->players, 0);
    local_p_data.game = game;
    pthread_create(&t2, NULL, thread_update_player, &local_p_data);
    pthread_create(&t1, NULL, thread_update_bombs, &local_p_data);
    pthread_create(&t1, NULL, thread_multiplayer, &local_p_data);

    game_loop(window, renderer, game);

    return 0;
}

void checkWin(Game *game, int *won, int *lost){

    if(get_list_postition(&game->players, 0)->alive==1 && get_list_postition(&game->players, 0)->next == NULL && game->player_count > 1 && *won==0){
        printf("you won\n");
        *won = 1;
    }
    if(get_list_postition(&game->players, 0)->alive==0 && *lost == 0 ){
        printf("you lost\n");
        *lost = 1;
    }
}

int game_loop(SDL_Window *window, SDL_Renderer *renderer, Game * game) {
    SDL_Rect startButton;
    bool running = true;
    SDL_Event event;
    SDL_MouseButtonEvent mouseEvent;
    int won =0; int lost = 0;
    while (running)
    {
        checkWin(game, &won , &lost);
        //Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);


        //render all element from bottom and up
        if(get_list_postition(&game->players, 0) != NULL) {

            render_grass(window);
            render_walls(window, game);
            render_boxes(window, game);
            render_bombs(window, game);
            render_players(window, game);
            render_explosion(window,game);
        }

        //Show whats rendered
        SDL_RenderPresent(renderer);

        if(SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                char msg[100]; // Send this to connected device
                sprintf(msg, "3 %d\n", get_list_postition(&game->players, 0)->id);
                client_send(game, &msg);

                running = false;
            }
        }
        //Spare the cpu, 16 =~ 60 fps
        SDL_Delay(16);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

SDL_Window * init_window(int w, int h, char *title) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            w,
            h,
            SDL_WINDOW_OPENGL);

    return window;
}

SDL_Renderer * init_renderer(SDL_Window *window) {

    SDL_Renderer *renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    return renderer;
}





