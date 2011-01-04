#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "vector2.h"
#include "plist.h"
#include "collison.h"

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
	new_node->pt.t = 10.1f;
	new_node->next = NULL;
	
	if(*list == NULL){
		*list = new_node;
		return 0;
	}
	
	new_node->next = *list;
	*list = new_node;
	return 0;
}

int plist_update(plist list, float dt){
	pnode *cycle = list;
	pnode *prev = NULL;
	
	while(cycle != NULL){
		
		cycle->pt.t -= dt;
		
		if (cycle->pt.t <= 0){
			if (prev == NULL){
				return 1;
			}
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
	return 0;
}

int plist_intersect(plist list, vector2 g1, vector2 g2, int line1){
	if(list == NULL)
		{return 0;}
	pnode *cycle = (line1)?(list->next):(list);
	if(cycle == NULL)
		{return 0;}
	pnode *nxt = cycle->next;
	
	while(nxt != NULL){		
		if(line_collison(g1, g2, nxt->pt.p, cycle->pt.p) && nxt->pt.t > 0.5){
			return 1;
		}
		cycle = cycle->next;
		nxt = nxt->next;
	}
	return 0;
}


void plist_render(plist list){
	pnode *cycle = list;
	float c[4];
	glGetFloatv(GL_CURRENT_COLOR, &c); 
	glPointSize(5);	
	glColor4f(c[0],c[1],c[2], 0.3);
	glLineWidth(10);
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	
	while(cycle != NULL){
		glVertex2f(cycle->pt.p.x, cycle->pt.p.y);
		cycle = cycle->next;
	}
		
	glEnd();
	glPopMatrix();
	/*glBegin(GL_QUAD_STRIP);
	while(cycle->next->next != NULL){
			vector2 perp1 = v2Add(cycle->pt.p, v2sMul(10,v2Rotate(M_PI_2, v2Unit(v2Sub(cycle->pt.p, cycle->next->pt.p)))));
			vector2 perp2 = v2Add(cycle->pt.p, v2sMul(10,v2Rotate(-M_PI_2, v2Unit(v2Sub(cycle->pt.p, cycle->next->pt.p)))));
			glVertex2f(perp1.x, perp1.y);
			glVertex2f(perp2.x, perp2.y);
			cycle=cycle->next;
	}
	glEnd();
	*/
	glLineWidth(1);
	cycle = list;
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	
	while(cycle != NULL){
		if(cycle->pt.t < 1.0f)
			{glColor4f(c[0]+0.25,c[1]+0.25,c[2]+0.25,cycle->pt.t);}
		else
			{glColor3f(c[0]+0.25,c[1]+0.25,c[2]+0.25);}
		
		glVertex2f(cycle->pt.p.x, cycle->pt.p.y);
		cycle = cycle->next;
	}
		
	glEnd();
	glPopMatrix();
	
	glColor3f(c[0],c[1],c[2]);
}

void plist_free(plist list){
	pnode *cycle = list;
	while(cycle != NULL){
		pnode *tmp = cycle->next;
		free(cycle);
		cycle = tmp;
	}	
}
