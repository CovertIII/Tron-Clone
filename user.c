#include <stdlib.h>
#include <stdio.h>
#include <GLUT/GLUT.h>
#include <enet/enet.h>
#include <tpl.h>
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
	return usr;
}

int user_add_list(user usr, user_info ui){
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
	user->ui.name = "No Name";
	user->ui.peer = peer;
	user->ui.status = status;
	user->ui.score = 0;
	user->ui.id = usr->user_num;
	user->next = NULL;
	usr->user_num++;
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
		return id;
	}
	if(cycle->next == NULL){
		usr->tail = prev;
		prev->next = NULL;
		free(cycle);
		return id;
	}
	prev->next = cycle->next;
	free(cycle);
	return id;
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
		return id;
	}
	if(cycle->next == NULL){
		usr->tail = prev;
		prev->next = NULL;
		free(cycle);
		return id;
	}
	prev->next = cycle->next;
	free(cycle);
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

void user_render(user usr){
	int i = 0;
	usernode *cycle = usr->head;
	char buf[50];
	sprintf(buf, "%d", usr->user_num);	
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(30, glutGet(GLUT_WINDOW_HEIGHT)-40, GLUT_BITMAP_HELVETICA_10, buf);
	glPopMatrix();

	while(cycle != NULL){
		char buf[50];
		sprintf(buf, "%s: %d", cycle->ui.name, cycle->ui.id);	
		glPushMatrix();
		glLoadIdentity();
		renderBitmapString(10, glutGet(GLUT_WINDOW_HEIGHT)-40-i*12, GLUT_BITMAP_HELVETICA_10, buf);
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
		if(cycle->ui.status== READY){
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
		cycle->ui.status = NOT_READY;
		cycle = cycle->next;
	}
}

void user_send_list(user usr, ENetPeer * newclient, int channel){
	user_info ui_p;
	usernode *cycle;
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
	enet_peer_send (newclient, channel, packet);	
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

void user_send_new_client(user usr, ENetPeer * peer, ENetHost *host, int channel){
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

	tn = tpl_map("S(siiiii)", &ui_p);
	tpl_pack(tn,0);
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);

	ENetPacket * packet;
	packet = enet_packet_create (addr, len, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast (host, channel, packet);
	free(addr);
}

void user_get_new_client(user usr, ENetPacket * packet){
	tpl_node * tn;
	user_info ui_p;
	tn = tpl_map("S(siiiii)", &ui_p);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	usernode * cycle;
	for(cycle = usr->head; cycle != NULL; cycle = cycle->next){
		if(ui_p.id == cycle->ui.id)
			{return;}
	}
	
	user_add_list(usr, ui_p);
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

void user_get_disconnect(user usr, ENetPacket * packet){
	tpl_node * tn;
	int id_p;
	tn = tpl_map("i", &id_p);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	user_remove_id(usr, id_p);
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

