#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLUT/GLUT.h>
#include <enet/enet.h>
#include "vector2.h"
#include "arena.h"
#include "chat.h"
#include "user.h"
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

static int get_input(client clnt, unsigned char key);

static void normal_keys(client clnt, unsigned key);

static void renderBitmapString(float x, float y, void *font, char *string); 

static void not_connected_keys(client clnt, unsigned key);

static void message_keys(client clnt, unsigned key);

static void name_keys(client clnt, unsigned key);

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
	clnt->mbuf[1] = '\0';	
	clnt->mbuf_num = 0;

	return clnt;
}

void client_connect(client clnt){
	clnt->game_mode = NORMAL;
	chat_add_message(clnt->c_chat, "Client", "Connection succesful.  Have fun.");
}

void client_disconnect(client clnt){
	clnt->game_mode = NOT_CONNECTED;
	user_free(clnt->c_users);
	clnt->c_users = user_init();
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
	glColor3f(1, 1, 1);
	chat_render(clnt->c_chat, 0);
	user_render(clnt->c_users);
	switch(clnt->game_mode){
		case NOT_CONNECTED:	
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 15, GLUT_BITMAP_HELVETICA_10, "Please enter the server you would like to connect to:\0");
			glPopMatrix();
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)- 27, GLUT_BITMAP_HELVETICA_10, clnt->mbuf);
			glPopMatrix();
			break;
		case MESSAGE:
			glPopMatrix();
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 15, GLUT_BITMAP_HELVETICA_10, "Message: ");
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 27, GLUT_BITMAP_HELVETICA_10, clnt->mbuf);
			glPopMatrix();
			break;
		case NAME:
			glPushMatrix();
			glLoadIdentity();
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 15, GLUT_BITMAP_HELVETICA_10, "What would you like your name to be?");
			renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT) - 27, GLUT_BITMAP_HELVETICA_10, clnt->mbuf);
			glPopMatrix();
			break;
	}
}

void client_keys(client clnt, unsigned char key){
	switch(clnt->game_mode){
		case NOT_CONNECTED:
			not_connected_keys(clnt, key);	
			break;
		case MESSAGE:
			message_keys(clnt, key);
			break;
		case NAME:
			name_keys(clnt, key);
			break;
		case NORMAL:
			normal_keys(clnt, key);
			break;
	}
}

void client_process_packets(client clnt, ENetEvent *event){
	ENetPacket * packet;
	switch(event->channelID){
			case 0:
				user_get_chat_message(clnt->c_users, clnt->c_chat, event->packet);
				break;
			case 1:
				user_get_list(clnt->c_users, event->packet);
				break;
			case 3:
				user_get_disconnect(clnt->c_users, event->packet);
				chat_add_message(clnt->c_chat, "Server", "Someone left");
				break;
	}
}

void client_free(client clnt){
}

int client_connection(client clnt){
	if(clnt->game_mode == NOT_CONNECTED) 
		{return 0;}
	else
		{return 1;}
}

static int get_input(client clnt, unsigned char key) {
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

static void normal_keys(client clnt, unsigned key){
	ENetEvent event;
	switch(key){
		case 'M':
		case 'm':
			clnt->game_mode = MESSAGE;
			break;
		case 'N':
		case 'n':
			clnt->game_mode = NAME;
			break;
		case 27:
			if(clnt->game_mode != NOT_CONNECTED){
				clnt->game_mode = NOT_CONNECTED;
				user_free(clnt->c_users);
				clnt->c_users = user_init();

				enet_peer_disconnect (clnt->enet_server, 0);
        	
			    /* Allow up to 3 seconds for the disconnect to succeed
			 	and drop any packets received packets.
			     */
				while (enet_host_service (clnt->enet_client, & event, 3000) > 0){
			    	switch (event.type){
			    		case ENET_EVENT_TYPE_RECEIVE:
			        		enet_packet_destroy (event.packet);
			        	break;
        	
			    		case ENET_EVENT_TYPE_DISCONNECT:
					   		printf("You left the server.\n");
							chat_add_message(clnt->c_chat, "Client", "You left the server.  Push <esc> again to exit the game");
			        	return;
			    	}
				}
        	
				/* We've arrived here, so the disconnect attempt didn't */
				/* succeed yet.  Force the connection down.             */
				enet_peer_reset (clnt->enet_server);
			}
			break;
	}
}

static void message_keys(client clnt, unsigned key){
	ENetPacket * packet;
	ENetAddress address;
	ENetEvent event;
	char disp [500];
	if(get_input(clnt, key)){
		clnt->mbuf[clnt->mbuf_num] = '\0';
		packet = enet_packet_create (clnt->mbuf, strlen (clnt->mbuf) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send (clnt->enet_server, 0, packet);
	
		clnt->mbuf_num = 0;
		clnt->mbuf[1] = '\0';	
		clnt->game_mode = NORMAL;
	}
	if(key == 27)
		{clnt->game_mode = NORMAL;}
}

static void name_keys(client clnt, unsigned key){
	ENetPacket * packet;
	ENetAddress address;
	ENetEvent event;
	char disp [500];
	if(get_input(clnt, key)){
		clnt->mbuf[clnt->mbuf_num] = '\0';
		packet = enet_packet_create (clnt->mbuf, strlen (clnt->mbuf) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send (clnt->enet_server, 1, packet);
	
		clnt->mbuf_num = 0;
		clnt->mbuf[1] = '\0';	
		clnt->game_mode = NORMAL;
	}
	if(key == 27)
		{clnt->game_mode = NORMAL;}
	
}

static void not_connected_keys(client clnt, unsigned key){
	ENetPacket * packet;
	ENetAddress address;
	ENetEvent event;
	char disp [500];
	//Connect to the server using the string in mbuf as the address upon hitting return.
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
			clnt->game_state = LOBBY;
			clnt->mbuf_num = 0;
			clnt->mbuf[1] = '\0';
		}
		else{
			enet_peer_reset (clnt->enet_server);
			sprintf(disp, "Connection to %s failed.", clnt->mbuf);
			chat_add_message(clnt->c_chat, "Client", disp);
			printf("Connection to %s failed.\n", clnt->mbuf);
			clnt->mbuf_num = 0;
			clnt->mbuf[1] = '\0';
		}
	}
	if (key == 27){
		exit(0);
	}
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

