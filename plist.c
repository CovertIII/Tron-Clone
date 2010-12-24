#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "vector2.h"
#include "plist.h"

typedef struct{
	vector2 p;
	float t;
} pt_type;


typedef struct pnode {
	pt_type pt;
	struct pnode * next;
} pnode;

int plist_add(plist *list, vector2 p){
	
	pnode *new_node;
	
	new_node = (pnode*)malloc(sizeof(pnode));
	if(new_node==NULL)
		{return 1;}
	new_node->pt.p = p;
	new_node->pt.t = 1.1f;
	new_node->next = NULL;
	
	if(*list == NULL){
		*list = new_node;
		return 0;
	}
	
	new_node->next = *list;
	*list = new_node;
	return 0;
}

void plist_update(plist list, float dt){
	pnode *cycle = list;
	pnode *prev = NULL;
	
	while(cycle != NULL){
		
		cycle->pt.t -= dt;
		
		if (cycle->pt.t <= 0){
			pnode *temp = cycle;
			prev->next = cycle->next;
			cycle = cycle->next;
			free(temp);
		}
		else{
			prev = cycle;
			cycle = cycle->next;	
		}
	}
}

void plist_render(plist list){
	pnode *cycle = list;
	
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	
	while(cycle != NULL){
		if(cycle->pt.t < 1.0f)
			{glColor4f(1,1,1, cycle->pt.t);}
		else
			{glColor3f(1,1,1);}
		
		glVertex2f(cycle->pt.p.x, cycle->pt.p.y);
		cycle = cycle->next;
	}
		
	glEnd();
	glPopMatrix();
}
