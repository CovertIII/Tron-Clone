#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include "vector2.h"
#include "particle_sys.h"

typedef struct{
	vector2 p;
	vector2 v;
	float t;
} particletype;

typedef struct particlenode {
	particletype p;
	struct particlenode * next;
} particlenode;

particles particles_init(vector2 pt1, vector2 pt2, vector2 v){
	particles pts;
	float length = v2Len(v2Sub(pt1, pt2));

	particlenode *cycle;
	particlenode *prev = NULL;	

	float i;
	for(i=0; i<=length; i += 0.1f){
		cycle = (particles)malloc(sizeof(particlenode));
		//if cycle is null free the memory allocated already, then make pts NULL;
		if(i==0)
			{pts = cycle;}
		if(prev != NULL) 
			{prev->next = cycle;}
		cycle->p.p = v2Add(pt2, v2sMul(i,v2Unit(v2Sub(pt1, pt2))));
		vector2 rv = {rand()%50-25, rand()%50-25};
		cycle->p.v = v2Add(v, rv);
		cycle->p.t = 2+(float)(rand()%11)/10.0f;
		cycle->next = NULL;
		prev = cycle;
	}
	return pts;
}

void particles_update(particles pts, float dt){
	if(pts == NULL)
		{return;}
	particlenode *cycle = pts;
	while(cycle != NULL){
		cycle->p.v.x += (-cycle->p.v.x)*dt;
		cycle->p.v.y += (-cycle->p.v.y)*dt;
		cycle->p.p.x += cycle->p.v.x*dt;
		cycle->p.p.y += cycle->p.v.y*dt;
		cycle->p.t -= dt;
		if(cycle->p.t <= 0){
			cycle->p.t = 0;
		}
		cycle = cycle->next;
	}
}


void particles_bound(particles pts, int x, int y){
	if(pts == NULL)
		{return;}
	particlenode *cycle = pts;
	while(cycle != NULL){
		if(cycle->p.p.x < 0){
			cycle->p.p.x = 0;
			cycle->p.v.x = -cycle->p.v.x;
		}
		if(cycle->p.p.x > x){
			cycle->p.p.x = x;
			cycle->p.v.x = -cycle->p.v.x;
		}
		if(cycle->p.p.y < 0){
			cycle->p.p.y = 0;
			cycle->p.v.y = -cycle->p.v.y;
		}
		if(cycle->p.p.y > y){
			cycle->p.p.y = y;
			cycle->p.v.y = -cycle->p.v.y;
		}
		cycle = cycle->next;
	}
}

void particles_render(particles pts){
	if(pts == NULL)
		{return;}
	particlenode *cycle = pts;
	float c[4];
	glGetFloatv(GL_CURRENT_COLOR, &c); 
	glPointSize(2);	

	glPushMatrix();
	glBegin(GL_POINTS);
	
	while(cycle != NULL){
		if(cycle->p.t < 1.0f)
			{glColor4f(c[0],c[1],c[2],cycle->p.t);}
		else
			{glColor3f(c[0],c[1],c[2]);}
		
		glVertex2f(cycle->p.p.x, cycle->p.p.y);
		cycle = cycle->next;
	}
		
	glEnd();
	glPopMatrix();
	glColor3f(c[0],c[1],c[2]);
}

void particles_free(particles pts){
	particlenode *cycle = pts;
	while(cycle != NULL){
		particlenode *tmp = cycle->next;
		free(cycle);
		cycle = tmp;
	}
}	
