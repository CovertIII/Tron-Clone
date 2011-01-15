#include <stdlib.h>
#include <stdio.h>
#include <enet/enet.h>
#include "user.h"

typedef struct{
	char name[80];
	int arena_plyr_id;
	int id;
	ENetPeer *peer;
	int status;
	int score;
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

int user_add(user usr, ENetPeer *peer, int status){
	usernode *user;
	user = (usernode*)malloc(sizeof(usernode));
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
	id = cycle->ui.arena_plyr_id;
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

void user_update_score(user usr, int arna_id, int score){
	usernode *cycle = usr->head;
	while(cycle->ui.arena_plyr_id != arna_id || cycle != NULL)
		{cycle = cycle->next;}
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

