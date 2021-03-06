//
// Created by martin on 4/21/16.
//

#include "client.h"



int initClient(Game *game)
{

    TCPsocket client;
    IPaddress ip;
    SDLNet_Init();

    // Our IP: 130.237.84.165 port: 22222
    if(SDLNet_ResolveHost(&ip,"130.237.84.165",22222)==-1) //Change loopback ip to our server IP
    {
        printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
        exit(3);
    }
    client = SDLNet_TCP_Open(&ip);
    while(!client) {
        client = SDLNet_TCP_Open(&ip);      //Open socket, if error occurs return to menu
        if (!client) {
            printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
            return 1;
        }
        SDL_Delay(1000);
    }
    SDLNet_SocketSet server = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(server, client);

    game->client = client;
    game->server = server;

    //Make sure we get the firs packet that defines local player
    SDL_Delay(50);
    client_recv(game);

    return 0;
}

void client_EXIT(TCPsocket client){

    SDLNet_TCP_Close(client);
    SDLNet_Quit();
}

void client_recv(Game *game){
    char tmp[1400];
    if(SDLNet_CheckSockets(game->server,0)>0 && SDLNet_SocketReady(game->client)){

        SDLNet_TCP_Recv(game->client, tmp, 1400);
        bool add = true;
        int type, id;
        //Check the type of message and who sent it
        sscanf(tmp, "%d %d",&type, &id);
        if (type == 1){
            printf("received start packet\n");
            int x,y, map;
            sscanf(tmp, "1 %d %d %d %d\n", &id, &x,&y, &map);

            // If there is no players locally, add first as local player, also get and set the map
            if(get_list_postition(&game->players, 0) == NULL)
            {
                create_player(&game->players, &game->player_count, x,y, id, game->localPlayerChar);
                get_list_postition(&game->players,0)->local = 1;  //This is how we know this is the local player

                switch(map) {
                    case 0:
                        printf("lvl1\n");
                        game->map = load_map("level1.map");
                        break;
                    case 1:
                        printf("lvl2\n");
                        game->map = load_map("level2.map");
                        break;

                    default:
                        printf("error with map");
                        break;
                }
            }

            //Check if players allready are exist locally
            for(int i=0; i < dlist_size(&game->players); i++)
            {
                if(get_list_postition(&game->players, i)->id == id ){
                    add = false;
                }
            }
            //If not add them
            if (add)
            {
                //Make sure no one spawn with the same color
                if(game->localPlayerChar == id)
                    create_player(&game->players, &game->player_count, x,y, id,get_list_postition(&game->players, 0)->id);
                else
                create_player(&game->players, &game->player_count, x,y, id,id);
            }


        }
        if (type == 2 && game->players.element != NULL){
            int tmp2;

            for(int i = 0; i < dlist_size(&game->players); i++){
                if(id== get_list_postition(&game->players,i)->id){
                    DlistElement *player = get_list_postition(&game->players,i);
                    sscanf(tmp, "2 %d %f %f %d %d\n", &tmp2, &player->anix, &player->aniY, &player->rotation, &player->moving);
                }
            }
        }
        if (type == 8 && game->players.element != NULL){
            printf("got message\n");
            game->gameRunning = 1;
        }
        if (type == 3){
            printf("recived lobby full\n");
            game->gameClosed =2;
        }
        if (type == 4){

            int x,y;
            sscanf(tmp, "4 %d %d %d\n", &id,&x,&y);
            printf("recived bomb packet: %d %d\n", x,y);

            struct _DlistElement *player = get_list_postition(&game->players,get_pos_from_id(&game->players, id));
            player_place_bomb(player, game, x,y);

        }

        if (type == 7){
            printf("recived death packet\n");
            sscanf(tmp, "7 %d \n", &id);

            dlist_removeElement(&game->players,get_pos_from_id(&game->players, id));

        }
        if (type == 9){
            printf("recived dc packet\n");
            sscanf(tmp, "9 %d \n", &id);

           dlist_removeElement(&game->players,get_pos_from_id(&game->players, id));
            game->player_count-=1;

        }
    }
}

void client_send(Game *game, char *msg){
    SDLNet_TCP_Send(game->client,  msg, (int)strlen(msg)+1);
}