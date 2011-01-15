#include <stdio.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>
#include <enet/enet.h>
#include "vector2.h"
#include "arena.h"
#include "user.h"
#include "chat.h"
#include "client.h"

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
	ENetPeer * enet_server;
	ENetHost * enet_client;
	int game_state;
	int game_mode;
	arena c_game;
	chat c_chat;
	user c_users;
	char mbuf[500];
	int mbuf_num;
	double ctimer;
	double timer;
} clienttype;

int get_input(client clnt, unsigned char key);

static void renderBitmapString(
						float x, 
						float y, 
						void *font,
						char *string); 


client client_init(ENetHost * enet_client){
	client clnt;

	clnt = (client)malloc(sizeof(clienttype));
	if(clnt == NULL){return NULL;}

	clnt->game_state = LOBBY;
	clnt->game_mode = NOT_CONNECTED;
	clnt->c_game = NULL;
	clnt->c_chat = chat_init();
	clnt->c_users = user_init();	
	clnt->timer = 0;
	clnt->ctimer = 0;
	clnt->enet_client = enet_client;
	clnt->mbuf_num = 0;

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
	clnt->ctimer += dt;
	clnt->mbuf[clnt->mbuf_num] = clnt->ctimer < 0.5 ? '|' : ' '; 
	if(clnt->ctimer > 1)
		{clnt->ctimer = 0;}
}

void client_render(client clnt){
	char *buf;
	glColor3f(0, 0, 0);
	chat_render(clnt->c_chat, 0);
	switch(clnt->game_mode){
		case NOT_CONNECTED:	
			glColor3f(0, 0, 0);
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 15, GLUT_BITMAP_TIMES_ROMAN_10, "Please enter the server you would like to connect to:\0");
			glPopMatrix();
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)- 27, GLUT_BITMAP_TIMES_ROMAN_10, clnt->mbuf);
			glPopMatrix();
			break;
		case MESSAGE:
			glPopMatrix();
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-15, GLUT_BITMAP_TIMES_ROMAN_10, clnt->mbuf);
			glPopMatrix();
			break;
		case NAME:
			glPushMatrix();
			glLoadIdentity();
			glColor3f(0, 1, 1);
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 3, GLUT_BITMAP_TIMES_ROMAN_10, "What would you like your name to be?");
			glPopMatrix();
			glPushMatrix();
			glLoadIdentity();
			glColor3f(0, 0, 0);
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-15, GLUT_BITMAP_TIMES_ROMAN_10, clnt->mbuf);
			glPopMatrix();
			break;
	}
}

void client_keys(client clnt, unsigned char key){
	ENetPacket * packet;
	ENetAddress address;
	ENetEvent event;
	char disp [500];

	switch(clnt->game_mode){
		case NOT_CONNECTED:
			if(get_input(clnt, key)){
				clnt->mbuf[clnt->mbuf_num] = '\0';
			
				enet_address_set_host (& address, clnt->mbuf);
				address.port = 5001;

				/* Initiate the connection. The third argument is the number of channels, the 4th is user supplied data.*/
				clnt->enet_server = enet_host_connect (clnt->enet_client, & address, 4, 0);    

				if (clnt->enet_server == NULL){
					fprintf (stderr,"No available peers for initiating an ENet connection.\n");
					exit (EXIT_FAILURE);
				}

				/* Wait up to 5 seconds for the connection attempt to succeed. */
				if (enet_host_service (clnt->enet_client, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
					sprintf(disp, "Connection to %s succeeded.\n", clnt->mbuf);
					printf("Connection to %s succeeded.\n", clnt->mbuf);
					chat_add_message(clnt->c_chat, "Client", disp);
					clnt->game_mode = NORMAL;	
					clnt->mbuf_num = 0;
					clnt->mbuf[1] = '\0';
				}
				else{
					enet_peer_reset (clnt->enet_server);
					sprintf(disp, "Connection to %s failed.", clnt->mbuf);
					chat_add_message(clnt->c_chat, "Client", disp);
					printf("Connection to %s failed.", clnt->mbuf);
					clnt->mbuf_num = 0;
					clnt->mbuf[1] = '\0';
				}
			}
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

int client_connection(client clnt){
	if(clnt->game_mode == NOT_CONNECTED) 
		{return 0;}
	else
		{return 1;}
}

int get_input(client clnt, unsigned char key) {
	int i;
	if(key==10 || key==13){
		return 1;
	}
	else if(key>=1 && key < 127 && clnt->mbuf_num < 500){
		clnt->mbuf[clnt->mbuf_num]=key;
		clnt->mbuf_num++;
		clnt->mbuf[clnt->mbuf_num+1] = '\0';
		return 0;
	}
	else if (key == 127 && clnt->mbuf_num > 0) {
		clnt->mbuf_num--;
		clnt->mbuf[clnt->mbuf_num+1] = '\0';
		return 0;
	}
	return 0;
}

static void renderBitmapString(
						float x, 
						float y, 
						void *font,
						char *string) {  
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}