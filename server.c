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

static void get_keys(server svr, ENetEvent * event);

typedef struct servertype{
	int game_state;
	ENetHost * enet_server;
	arena s_game;
	chat s_chat;
	user s_users;
	double timer;
	double update_timer;
	int t;
} servertype;

server server_init(ENetHost * enet_server){
	server svr;
	svr = (server)malloc(sizeof(servertype));
	svr->game_state = LOBBY;
	svr->update_timer = 0.0f;
	svr->s_users = user_init(); 
	svr->enet_server = enet_server;
	svr->s_chat = chat_init();
	svr->t = 0;
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
	svr->update_timer += dt;
	if(svr->update_timer > 3){
		svr->update_timer = 0.0f;
		user_send_list(svr->s_users, svr->enet_server, 1);
	}
	switch(svr->game_state){
		case LOBBY:
			if(svr->s_users != NULL && user_number(svr->s_users) > 0 && user_check_states(svr->s_users) == 0){
				svr->game_state = PREGAME;
				svr->timer = 3.0f;
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
			}
			break;
		case GAME:
			arena_update(svr->s_game, dt);
			arena_send_update(svr->s_game, svr->enet_server, 4);
			if(arena_player_status(svr->s_game) <= 1){
				int winner_id = arena_winner(svr->s_game);
				user_update_score(svr->s_users, winner_id, 1);
				svr->game_state = POSTGAME;
				svr->timer = 5.0f;
			}
			break;
		case POSTGAME:
			arena_update(svr->s_game, dt);
			arena_send_update(svr->s_game, svr->enet_server, 4);
			svr->timer -= dt;
			if(svr->timer < 0){
				arena_free(svr->s_game);
				svr->game_state = LOBBY;
				user_all_not_ready(svr->s_users);
				server_send_gm_free(svr, 5);
				user_send_list(svr->s_users, svr->enet_server, 1);
			}
			break;
	}
}


void server_process_packet(server svr, ENetEvent event){
	ENetPacket * packet;
	switch(event.channelID){
			case 0:
				user_send_chat_message(svr->s_users, &event, svr->enet_server, 0);
				break;
			case 1:
				user_change_name_send(svr->s_users, svr->enet_server, &event, 1);
				break;
			case 2:
				user_sget_send_pstate(svr->s_users, svr->enet_server,  &event, 1);
				break;
			case 3:
				if((svr->game_state == GAME || svr->game_state == POSTGAME) &&
					svr->s_game != NULL){
					get_keys(svr, &event);
				}
				break;

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

static void get_keys(server svr, ENetEvent * event){
	tpl_node *tn;
	void *addr;
	size_t len;
	
	int id;
	char lr, ud, t, s;

	tn = tpl_map("cccc", &lr, &ud, &t, &s);
	tpl_load(tn, TPL_MEM, event->packet->data, event->packet->dataLength);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	id = user_peer_aid(svr->s_users, event->peer);

	if (lr == -1)
		{s ? arena_ply_turn(svr->s_game, id, 7) : arena_ply_turn(svr->s_game, id, 3);}
	else if (lr == 1)
		{s ? arena_ply_turn(svr->s_game, id, -7) : arena_ply_turn(svr->s_game, id, -3);}
	else if (lr == 0)
		{ arena_ply_turn(svr->s_game, id, 0);}
	if (ud == -1)
		{arena_ply_speed(svr->s_game, id, -500);}
	else if (ud == 1)
		{arena_ply_speed(svr->s_game, id,  500);}
	else if (ud == 0)
		{arena_ply_speed(svr->s_game, id, 0);}
	t ? arena_plyr_t_off(svr->s_game, id) : arena_plyr_t_on(svr->s_game, id);
}

