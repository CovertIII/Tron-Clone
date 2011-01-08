#include <stdio.h>
#include <stdlib.h>
#include <enet.h>
#include "vector2.h"
#include "arena.h"
#include "server.h"

#define LOBBY 0
#define PREGAME 1
#define GAME 2
#define POSTGAME 3

typedef struct {
	int game_state;
	arena s_game;
	//chat s_chat;
	users s_users;
	double timer;
} servertype;

server server_init(void){
	server svr;
	svr = (server)malloc(sizeof(servertype));
	svr->game_state = LOBBY;
	svr->s_user = user_init(void); 
	svr->s_chat = chat_init(void);
	return svr;
}

/* Maybe send something to let the client know a new user joined in this funtion?
 * Also send the state of the server and or arena to the newly connected client
 */
void server_add_user(server svr, ENetPeer *peer){
	switch(svr->game_date){
		case LOBBY:
			user_add(svr->s_user, peer, NOT_READY);
			break;
		case PREGAME:
		case GAME:
		case POSTGAME:
			user_add(svr->s_user, peer, WAITING);
			break;
	}
	/* TODO: Send information to the new client about the world:
	 *         o list of the users including himself
	 *         o state of the server
	 *         o if the game is in progress, send a message to wait for the game to be over.
	*/
	//TODO: Send information to the rest of the clients to let them know a new person has joined.
}

void server_remove_user(server svr, ENetPeer *peer){ 
	switch(svr->game_state){
		case LOBBY:
			user_remove(svr->s_user, peer);
			break;
		case PREGAME:
		case GAME:
		case POSTGAME:
			int id = user_remove(svr->s_user, peer);
			arena_kill_player(svr->s_game, id);
			break;
	}
	//TODO: send somethign to let clients know a user has disconnected
}
void server_update(server svr, double dt){
	switch(svr->game_state){
		case LOBBY:
			if(svr->s_user != NULL && user_number(svr->user) > 0 && user_check_state(svr->s_user) == 0){
				svr->game_state = PREGAME;
				svr->timer = 5.0f;
				int ply_num = user_set_arena_id(svr->s_user);
				svr->s_game = arena_init(ply_num, 0, 800, 600);
				//TODO:  Maybe let the client know that we are switching to the pregame state
				//       and to set its timer to the server pregame timer.
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
			if(arean_player_status(svr->game_state) <= 1){
				int winner_id = arena_winner(svr->s_game);
				user_update_score(svr->s_users, winner_id, 1);
				svr->game_state = POSTGAME;
				svr->timer = 2.0f;
				//TODO:  Let clients know the winner name and that we are in a POSTGAME state now.
			}
			break;
		case POSTGAME:
			arena_update(svr->s_game, dt);
			svr->timer -= dt;
			if(svr->timer < 0){
				arena_free(svr->s_game);
				svr->game_state = LOBBY;
				//TODO: Let clients know that we are in the lobby now.
			}
	}


void server_send_updates(server svr);

void server_process_packet(server svr, char *message);
