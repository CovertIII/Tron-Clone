#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tpl.h>
#include <enet/enet.h>
#include "vector2.h"
#include "arena.h"
#include "chat.h"
#include "user.h"
#include "server.h"

#define LOBBY 0
#define PREGAME 1
#define GAME 2
#define POSTGAME 3



void server_send_gm_init(server svr, double timer, int ply_num, int x_bd, int y_bd, enet_uint8 channel);

void server_send_gm_free(server svr, enet_uint8 channel);

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
	user_send_list(svr->s_users, svr->enet_server, 1);
}

void server_remove_user(server svr, ENetPeer *peer){ 
	int id;
	id = user_remove(svr->s_users, peer);
	//lets other users know who disconnected
	user_send_disconnect(id, 3, svr->enet_server);
}
void server_update(server svr, double dt){
	switch(svr->game_state){
		case LOBBY:
			if(svr->s_users != NULL && user_number(svr->s_users) > 0 && user_check_states(svr->s_users) == 0){
				svr->game_state = PREGAME;
				svr->timer = 5.0f;
				int ply_num = user_set_arena_id(svr->s_users);
				svr->s_game = arena_init(ply_num, 0, 800, 600);
				// let the client know that we are switching to the pregame state
				//       and to set its timer to the server pregame timer.
				//       tell the client to init an arena with the ply_num (AI currently not implemented)
				server_send_gm_init(svr, svr->timer, ply_num, 800, 600, 2);
			}
			break;
		case PREGAME:
			svr->timer -= dt;
			arena_send_update(svr->s_game, svr->enet_server, 4);
			if(svr->timer < 0){
				svr->game_state = GAME;
				//TODO: let clients know we are in game state now.
			}
			break;
		case GAME:
			arena_update(svr->s_game, dt);
			arena_send_update(svr->s_game, svr->enet_server, 4);
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
			arena_send_update(svr->s_game, svr->enet_server, 4);
			svr->timer -= dt;
			if(svr->timer < 0){
				arena_free(svr->s_game);
				svr->game_state = LOBBY;
				user_all_not_ready(svr->s_users);
				server_send_gm_free(svr, 5);
				//TODO: Let clients know that we are in the lobby now.
				//TODO: send all user data to clients to ensure the games are in snyc.
			}
			break;
	}
}


void server_process_packet(server svr, ENetEvent event){
	ENetPacket * packet;
	printf("Channel Limit %d\n", svr->enet_server->channelLimit);
	switch(event.channelID){
			case 0:
				user_send_chat_message(svr->s_users, &event, svr->enet_server, 0);
				break;
			case 1:
				user_change_name_send(svr->s_users, svr->enet_server, &event, 1);
				break;
			case 2:
				user_sget_send_pstate(svr->s_users, svr->enet_server,  &event, 1);
	}
}

void server_send_gm_init(server svr, double timer, int ply_num, int x_bd, int y_bd, enet_uint8 channel){
	tpl_node *tn;
	void *addr;
	size_t len;

	tn = tpl_map("fiii", &timer, &ply_num, &x_bd, &y_bd);
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	
	//broadcast the package	
	ENetPacket * packet;
	packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (svr->enet_server, channel, packet);
	free(addr);
}

void server_send_gm_free(server svr, enet_uint8 channel){
	ENetPacket * packet;
	packet = enet_packet_create ("HI", sizeof("Hi"+1), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (svr->enet_server, channel, packet);
}

