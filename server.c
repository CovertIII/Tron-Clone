#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <enet/enet.h>
#include "vector2.h"
#include "arena.h"
#include "user.h"
#include "chat.h"
#include "server.h"

#define LOBBY 0
#define PREGAME 1
#define GAME 2
#define POSTGAME 3

typedef struct servertype{
	int game_state;
	ENetHost * enet_server;
	arena s_game;
	chat s_chat;
	user s_users;
	double timer;
} servertype;

server server_init(ENetHost * enet_server){
	server svr;
	svr = (server)malloc(sizeof(servertype));
	svr->game_state = LOBBY;
	svr->s_users = user_init(); 
	svr->enet_server = enet_server;
	svr->s_chat = chat_init();
	return svr;
}

/* Maybe send something to let the client know a new user joined in this funtion?
 * Also send the state of the server and or arena to the newly connected client
 */
void server_add_user(server svr, ENetPeer *peer){
	switch(svr->game_state){
		case LOBBY:
			user_add(svr->s_users, peer, NOT_READY);
			break;
		case PREGAME:
		case GAME:
		case POSTGAME:
			user_add(svr->s_users, peer, WAITING);
			break;
	}
	user_send_list(svr->s_users, peer, 1);
	user_send_new_client(svr->s_users, peer, svr->enet_server, 2);
	/* TODO: Send information to the new client about the world:
	 *         o list of the users including himself
	 *         o state of the server
	 *         o if the game is in progress, send a message to wait for the game to be over.
	*/
	//TODO: Send information to the rest of the clients to let them know a new person has joined.
}

void server_remove_user(server svr, ENetPeer *peer){ 
	int id;
	id = user_remove(svr->s_users, peer);
	//lets other users know who disconnected
	user_send_disconnect(id, 3, svr->enet_server);
}
void server_update(server svr, double dt){
	printf("GS: %d\n", svr->game_state);
	switch(svr->game_state){
		case LOBBY:
			if(svr->s_users != NULL && user_number(svr->s_users) > 0 && user_check_states(svr->s_users) == 0){
				svr->game_state = PREGAME;
				svr->timer = 5.0f;
				int ply_num = user_set_arena_id(svr->s_users);
				svr->s_game = arena_init(ply_num, 0, 800, 600);
				//TODO:  Maybe let the client know that we are switching to the pregame state
				//       and to set its timer to the server pregame timer.
				//       tell the client to init an arena with the ply_num (AI currently not implemented)
			}
			break;
		case PREGAME:
			svr->timer -= dt;
			if(svr->timer < 0){
				svr->game_state = GAME;
				//TODO: let clients know we are in game state now.
			}
			break;
		case GAME:
			arena_update(svr->s_game, dt);
			//TODO: send the game updates here. (if someone dies send that info, put don't send the particle system)
			if(arena_player_status(svr->s_game) <= 1){
				int winner_id; //= arena_winner(svr->s_game);
				user_update_score(svr->s_users, winner_id, 1);
				svr->game_state = POSTGAME;
				svr->timer = 2.0f;
				//TODO:  Let clients know the winner name and that we are in a POSTGAME state now.
			}
			break;
		case POSTGAME:
			arena_update(svr->s_game, dt);
			//TODO: send game updates here as well.
			svr->timer -= dt;
			if(svr->timer < 0){
				arena_free(svr->s_game);
				svr->game_state = LOBBY;
				user_all_not_ready(svr->s_users);
				//TODO: Let clients know that we are in the lobby now.
				//TODO: send all user data to clients to ensure the games are in snyc.
			}
			break;
	}
}


void server_process_packet(server svr, ENetEvent event){
	ENetPacket * packet;
	switch(event.channelID){
			case 0:
				packet = enet_packet_create (event.packet->data, strlen (event.packet->data) + 1, ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast (svr->enet_server, 0, packet);
				break;
			case 1:
				break;
	}
}
