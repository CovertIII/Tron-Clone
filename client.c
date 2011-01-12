#include <stdio.h>
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

#define NOT_CONNECTED 0
#define NORMAL 1
#define MESSAGE 2
#define NAME 3
#define COMMAND 4

typedef struct clienttype{
	int game_state;
	int game_mode;
	arena c_game;
	chat c_chat;
	user c_users;
	char mbuf[500];
	double timer;
} clienttype;

client client_init(void){
	client clnt;

	clnt = (client)malloc(sizeof(clienttype));
	if(clnt == NULL){return NULL;}

	clnt->game_state = LOBBY;
	clnt->game_mode = NOT_CONNECTED;
	clnt->c_game = NULL;
	clnt->c_chat = chat_init();
	clnt->c_users = user_init();	
	clnt->timer = 0;

	return clnt;
}

void client_connect(client clnt){
	clnt->game_mode = NORMAL;
	chat_add_message(clnt->c_chat, "Client", "Connection succesful.  Have fun.");
}

void client_disconnect(client clnt){
	clnt->game_mode = NOT_CONNECTED;
	chat_add_message(clnt->c_chat, "Client", "You're disconnected from the server.");
}

void client_update(client clnt, double dt){
}

void client_render(client clnt){
	
}

void client_keys(client clnt, unsigned char key){
	switch(clnt->game_mode){
		case NOT_CONNECTED:

			break;
		case MESSAGE:

			break;
		case NAME:

			break;
		case NORMAL:
			break;
	}
}

void client_process_packets(ENetEvent *event){
}

void client_free(client clnt){
}


void type(unsigned char key) 
{
	int i;
	if(key==10 || key==13){
		buf[buf_num] = NULL;
		strcpy(trans,buf);
		buf_num=0;
		tran_in = 1;
		message_mode = OFF;
	}
	else if(key>=1 && key < 127 && buf_num < 97){
		buf[buf_num]=key;
		buf_num++;
	}
	else if (key == 127 && buf_num>0) {
		buf_num--;
	}
}
