#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLUT/GLUT.h>
#include "chat.h"

typedef struct {
	char *name;
	char *message;
} chat_data;

typedef struct chat_node {
	chat_data cd;
	struct chat_node * next;
} chat_node;

typedef struct chattype {
	chat_node *head;
	chat_node *tail;
	int m_num; //number of messages;
} chattype;

static void renderBitmapString(
						float x, 
						float y, 
						void *font,
						char *string); 


chat chat_init(void){
	chat cht;

	cht = (chat)malloc(sizeof(chattype));

	if(cht == NULL){
		return NULL;
	}

	cht->head = NULL;
	cht->tail = NULL;
	cht->m_num = 0;

	return cht;
}

int chat_add_message(chat cht, char *name, char *message){
	chat_node *temp;

	temp = (chat_node*)malloc(sizeof(chat_node));

	if(temp == NULL) {return 1;}

	temp->cd.name = (char*)malloc(strlen(name)+1);
	if(temp->cd.name == NULL){return 1;}
	strcpy(temp->cd.name, name);

	temp->cd.message = (char*)malloc(strlen(message)+1);
	if(temp->cd.message == NULL) {return 1;}
	strcpy(temp->cd.message, message);

	if(cht->head == NULL){
		cht->head = temp;
	  	temp->next = NULL;
	  	return 0;
	}
	temp->next = cht->head;
	cht->head = temp;
	return 0;
}

void chat_render(chat cht, int type){
	int i=0;
	chat_node *cycle = cht->head;
	
	while(cycle != NULL){
		char *buf;
		buf = (char*)malloc(strlen(cycle->cd.name)+strlen(cycle->cd.message)+5);
		sprintf(buf, "%s: %s", cycle->cd.name, cycle->cd.message);	
		glPushMatrix();
		glLoadIdentity();
		renderBitmapString(10, 10 + 12*i, GLUT_BITMAP_HELVETICA_10, buf);
		glPopMatrix();
		free(buf);
		cycle = cycle->next;
		++i;
	}	
}

void chat_free(chat cht){
	chat_node *cycle = cht->head;

	while(cycle != NULL){
		free(cycle->cd.name);
		free(cycle->cd.message);
		chat_node *tmp = cycle->next;
		free(cycle);
		cycle = tmp;
	}
	free(cht);
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


