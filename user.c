#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <enet/enet.h>
#include <tpl.h>
#include "chat.h"
#include "user.h"

typedef struct{
	char *name;
	int arena_plyr_id;
	int id;
	int ping;
	int status;
	int score;
	ENetPeer *peer;
} user_info;

typedef struct usernode{
	user_info ui;
	struct usernode * next;
} usernode;

typedef struct usertype{
	usernode* head;
	usernode* tail;
	int user_num;
	int user_id;
} usertype;

static void renderBitmapString(float x, float y, void *font, char *string); 

user user_init(void){
	user usr;
	usr = (user)malloc(sizeof(usertype));
	if(usr == NULL)
		{return NULL;}
	usr->head = NULL;
	usr->tail = NULL;
	usr->user_num = 0;
	usr->user_id = 0;
	return usr;
}

int user_add_list(user usr, user_info ui){
	usernode * cycle;
	for(cycle = usr->head; cycle != NULL; cycle = cycle->next){
		if(ui.id == cycle->ui.id){
			cycle->ui = ui;
			return;
		}
	}
	usernode *user;
	usr->user_num++;
	user = (usernode*)malloc(sizeof(usernode));
	user->ui = ui;
	user->next = NULL;
	if(usr->head == NULL){
		usr->head = usr->tail = user;
		return;
	}
	usr->tail->next = user;
	usr->tail = user;
}

int user_add(user usr, ENetPeer *peer, int status){
	usernode *user;
	user = (usernode*)malloc(sizeof(usernode));
	user->ui.name = (char*)malloc(sizeof(char)*8);
	strcpy(user->ui.name, "No Name");
	user->ui.peer = peer;
	user->ui.status = status;
	user->ui.score = 0;
	user->ui.id = usr->user_id;
	user->next = NULL;
	usr->user_num++;
	usr->user_id++;
	if(usr->head == NULL){
		usr->head = usr->tail = user;
		return;
	}
	usr->tail->next = user;
	usr->tail = user;
}

int user_remove_id(user usr, int id){
	usernode *cycle = usr->head;
	usernode *prev = NULL;
	if(cycle == NULL){return -1;}
	while(cycle != NULL){
		if(cycle->ui.id == id)
			 {break;}	
		prev = cycle;
		cycle = cycle->next;
	}
	if(cycle == NULL)
		{return -1;}
	id = cycle->ui.id;
	if(prev == NULL){
		usr->head = cycle->next;
		if(cycle->next == NULL)
			{usr->tail = NULL;}
		free(cycle);
		usr->user_num--;
		return id;
	}
	if(cycle->next == NULL){
		usr->tail = prev;
		prev->next = NULL;
		free(cycle);
		usr->user_num--;
		return id;
	}
	prev->next = cycle->next;
	free(cycle);
	usr->user_num--;
	return id;
}

char * user_nameby_id(user usr, int id){
	usernode *cycle = usr->head;
	while(cycle != NULL){
		if(cycle->ui.id == id)
			{return cycle->ui.name;}
		cycle = cycle->next;
	}
}

char * user_nameby_aid(user usr, int id){
	usernode *cycle = usr->head;
	while(cycle != NULL){
		if(cycle->ui.arena_plyr_id == id)
			{return cycle->ui.name;}
		cycle = cycle->next;
	}
}

int user_remove(user usr, ENetPeer *peer){
	usernode *cycle = usr->head;
	usernode *prev = NULL;
	int id;
	if(cycle == NULL){return -1;}
	while(cycle != NULL){
		if(cycle->ui.peer == peer)
			 {break;}	
		prev = cycle;
		cycle = cycle->next;
	}
	if(cycle == NULL)
		{return -1;}
	id = cycle->ui.id;
	if(prev == NULL){
		usr->head = cycle->next;
		if(cycle->next == NULL)
			{usr->tail = NULL;}
		free(cycle);
		usr->user_num--;
		return id;
	}
	if(cycle->next == NULL){
		usr->tail = prev;
		prev->next = NULL;
		free(cycle);
		usr->user_num--;
		return id;
	}
	prev->next = cycle->next;
	free(cycle);
	usr->user_num--;
	return id;
}

void user_free(user usr){
	if(usr->head == NULL){
		free(usr);
		return;
	}
	usernode *cycle = usr->head;
	while(cycle->next != NULL){
		usernode *tmp = cycle->next;
		free(cycle);
		cycle = tmp;
	}
	free(usr);
}	

void user_render(user usr, int x, int y){
	int i = 0;
	usernode *cycle = usr->head;
	char buf[50];
	sprintf(buf, "User     Score    Status    Ping");	
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(10, y-40, GLUT_BITMAP_HELVETICA_10, buf);
	glPopMatrix();

	while(cycle != NULL){
		char buf[50];
		char status[10];
		switch(cycle->ui.status){
			case 0:
				strcpy(status, "Not Ready");
				break;
			case 1:
				strcpy(status, "Ready");
				break;
			case 2:
				strcpy(status, "Playing");
				break;
			case 3:
				strcpy(status, "Waiting");
				break;
			case 4:
				strcpy(status, "Idle");
				break;
		}
		glColor3f((float)i/(float)usr->user_num,0.2,1-(float)i/(float)usr->user_num);

		glPushMatrix();
		glLoadIdentity();
		sprintf(buf, "%s", cycle->ui.name);	
		renderBitmapString(10, y-52-i*12, GLUT_BITMAP_HELVETICA_10, buf);
		sprintf(buf, "%d", cycle->ui.score);	
		renderBitmapString(60, y-52-i*12, GLUT_BITMAP_HELVETICA_10, buf);
		sprintf(buf, "%s", status);	
		renderBitmapString(80, y-52-i*12, GLUT_BITMAP_HELVETICA_10, buf);
		sprintf(buf, "%d", cycle->ui.ping);	
		renderBitmapString(150, y-52-i*12, GLUT_BITMAP_HELVETICA_10, buf);
		glPopMatrix();
		cycle = cycle->next;
		i++;
	}	
}

void user_update_score(user usr, int arna_id, int score){
	usernode *cycle = usr->head;
	while(cycle != NULL){
		if(cycle->ui.arena_plyr_id == arna_id){break;}
		cycle = cycle->next;
	}
	if(cycle == NULL)
		{return;}
	cycle->ui.score += score;
}

void user_set_ready(user usr, ENetPeer *peer){
	usernode *cycle = usr->head;
	while(cycle->ui.peer != peer || cycle != NULL)
		{cycle = cycle->next;}
	if(cycle == NULL)
		{return;}
	cycle->ui.status = READY;
}

int user_check_states(user usr){
	int state = 0;
	usernode *cycle = usr->head;
	while(cycle != NULL){
		if(cycle->ui.status == NOT_READY)
			{state++;}
		cycle = cycle->next;
	}
	return state;
}

int user_number(user usr){
	return usr->user_num;
}

int user_set_arena_id(user usr){
	int arena_id = 0;
	usernode *cycle = usr->head;
	while(cycle != NULL){
		if(cycle->ui.status == READY){
			cycle->ui.arena_plyr_id = arena_id;
			cycle->ui.status = PLAYING;
			arena_id++;
		}
		cycle = cycle->next;
	}
	return arena_id;
}

void user_all_not_ready(user usr){
	usernode *cycle = usr->head;
	while(cycle != NULL){
		if(cycle->ui.status != IDLE){
			cycle->ui.status = NOT_READY;
		}
		cycle = cycle->next;
	}
}

void user_send_list(user usr, ENetHost * host, int channel){
	//sets the ping to the right amount
	usernode *cycle = usr->head;
	while(cycle != NULL){
		cycle->ui.ping = cycle->ui.peer->roundTripTime;
		cycle = cycle->next;
	}
	
	//functino as normal
	user_info ui_p;
	tpl_node *tn;
	void *addr;
	size_t len;

	tn = tpl_map("A(S(siiiii))", &ui_p);
	for(cycle = usr->head; cycle != NULL; cycle = cycle->next){
		ui_p = cycle->ui;
		tpl_pack(tn, 1);
	}
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	
	ENetPacket * packet;
	packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (host, channel, packet);
	free(addr);
}	

void user_get_list(user usr, ENetPacket * packet){
	tpl_node * tn;
	user_info ui_p;
	tn = tpl_map("A(S(siiiii))", &ui_p);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	while( tpl_unpack(tn,1) > 0) { 
		user_add_list(usr, ui_p);
	}
	tpl_free(tn);
}

void user_send_new_client(user usr, ENetPeer * peer, ENetHost *host, enet_uint8 channel){
	user_info ui_p;
	usernode *cycle;
	for(cycle = usr->head; cycle != NULL; cycle = cycle->next){
		if(cycle->ui.peer == peer){
			ui_p = cycle->ui;
			break;
		}
	}
	tpl_node *tn;
	void *addr;
	size_t len;

	tn = tpl_map("A(S(siiiii))", &ui_p);
	tpl_pack(tn, 1);
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	
	ENetPacket * packet;
	packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (host, channel, packet);
	free(addr);
}

void user_send_disconnect(int id, int channel, ENetHost * host){
	int id_p;
	tpl_node *tn;
	void *addr;
	size_t len;

	tn = tpl_map("i", &id_p);
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	
	ENetPacket * packet;
	packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (host, channel, packet);
	free(addr);
}

void user_get_disconnect(user usr, ENetPacket * packet, chat cht){
	tpl_node * tn;
	int id_p;
	tn = tpl_map("i", &id_p);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	char * name = user_nameby_id(usr, id_p);
	user_remove_id(usr, id_p);
	char buf[200];
	sprintf(buf, "%s left the server", name);
	chat_add_message(cht, "Server", buf); 
}
	
void user_change_name_send(user usr, ENetHost * server,  ENetEvent * event, enet_uint8 channel){
	usernode * cycle;
	for (cycle = usr->head; cycle != NULL; cycle = cycle->next){
		//find the user that equals the event peer
		if(cycle->ui.peer == event->peer){
			char *tmp;
			tmp = (char*)malloc(event->packet->dataLength);
			if(tmp == NULL){fprintf(stderr, "Out of memory"); return;}
			//copy the string in packet data to user name
			strcpy(tmp, event->packet->data);
			free(cycle->ui.name);
			cycle->ui.name = tmp;

			//send the updated user via send_new_client	
			user_send_new_client(usr,cycle->ui.peer, server, channel);
			return;
		}
	}
	return;
}

void user_send_chat_message(user usr, ENetEvent * event, ENetHost * host, enet_uint8 channel){
	//find id of the peer who sent the chat message
	usernode * cycle;
	int id;
	for (cycle = usr->head; cycle !=NULL; cycle = cycle->next){
		if(cycle->ui.peer == event->peer){
			id = cycle->ui.id;
			break;
		}
	}
	if(cycle == NULL){return;}

	//pack up the id and message
	tpl_node *tn;
	void *addr;
	size_t len;

	tn = tpl_map("is", &id, &event->packet->data);
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	
	//broadcast the package	
	ENetPacket * packet;
	packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (host, channel, packet);
	free(addr);
}

void user_get_chat_message(user usr, chat cht, ENetPacket *packet){
	int id;
	char *msg;
	tpl_node * tn;
	tn = tpl_map("is", &id, &msg);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	usernode *cycle;
	for (cycle = usr->head; cycle !=NULL; cycle = cycle->next){
		if(cycle->ui.id == id){
			chat_add_message(cht, cycle->ui.name, msg);	
			return;
		}
	}
}

void user_sget_send_pstate(user usr, ENetHost * server,  ENetEvent * event, enet_uint8 channel){
	usernode * cycle;
	for (cycle = usr->head; cycle != NULL; cycle = cycle->next){
		//find the user that equals the event peer
		if(cycle->ui.peer == event->peer){
			if(cycle->ui.status == NOT_READY){
				cycle->ui.status = READY;
			}
			else if(cycle->ui.status == READY){
				cycle->ui.status = IDLE;
			}
			else if(cycle->ui.status == IDLE){
				cycle->ui.status = NOT_READY;
			}

			//send the updated user via send_new_client	
			user_send_new_client(usr,cycle->ui.peer, server, channel);
			return;
		}
	}
	return;
}

void user_send_arena_ids(user usr, int channel){
	usernode * cycle;
	for (cycle = usr->head; cycle != NULL; cycle = cycle->next){
		int id_p = cycle->ui.status != IDLE ? cycle->ui.arena_plyr_id : 0;
		tpl_node *tn;
		void *addr;
		size_t len;

		tn = tpl_map("i", &id_p);
		tpl_pack(tn, 0);
		tpl_dump(tn, TPL_MEM, &addr, &len);
		tpl_free(tn);
		
		ENetPacket * packet;
		packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send (cycle->ui.peer, channel, packet);
		free(addr);
	}
}

int user_get_arena_id(ENetPacket * packet){
	int id;
	tpl_node * tn;
	tn = tpl_map("i", &id);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	return id;
}

void user_disconnect_all(user usr, ENetHost * host){
	usernode * cycle;
	ENetEvent event;
	for (cycle = usr->head; cycle != NULL; cycle = cycle->next)
	{
		int connection = 1;
		enet_peer_disconnect (cycle->ui.peer, 0);
		/* Allow up to 3 seconds for the disconnect to succeed
		and drop any packets received packets.*/
		while (enet_host_service (host, & event, 3000) > 0){
			switch (event.type){
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy (event.packet);
				break;
	
				case ENET_EVENT_TYPE_DISCONNECT:
					connection = 0;
					break;
			}
		}
	
		/* We've arrived here, so the disconnect attempt didn't */
		/* succeed yet.  Force the connection down.             */
		if(connection)
			{enet_peer_reset (cycle->ui.peer);}
	}
}

int user_peer_aid(user usr, ENetPeer * peer){
	usernode * cycle;
	for(cycle = usr->head; cycle != NULL; cycle = cycle->next){
		if(cycle->ui.peer == peer){
			return cycle->ui.arena_plyr_id;
		}
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

