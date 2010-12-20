#include "player.h"
#include "vector2.h"
#include "math.h"
//#include "plist.h"
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define VELOCITY 100
#define LENGTH 15

typedef struct playertype {
	vector2 p;
	vector2 v;
	float vel;
	float th;
	float w;
	float length;
//	plist trail;
} playertype;

player player_init(int x, int y, double th){
	player plyr;

	plyr = (player)malloc(sizeof(playertype));
	if(plyr == NULL)
		{return NULL;}

	plyr->p.x=x;
	plyr->p.y=y;
	plyr->vel = VELOCITY;
	plyr->th = th;
	plyr->v.x = -VELOCITY*cos(th);
	plyr->v.y = VELOCITY*sin(th);
	plyr->length = LENGTH;
	
	return plyr;
}

void player_update(player plyr, double dt){
	plyr->th += plyr->w*dt;

	plyr->v.x = VELOCITY*cos(plyr->th);
	plyr->v.y = VELOCITY*sin(plyr->th);

	plyr->p.x += plyr->v.x*dt;
	plyr->p.y += plyr->v.y*dt;
}

void player_turn(player plyr, int dir){
	plyr->w = dir;
}

void player_render(player plyr){
	glPushMatrix();
	glLineWidth(3.0);
	glBegin(GL_LINES);
		glVertex2f(plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y);
		glVertex2f(-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y);
	glEnd();
	glPopMatrix();
}

void player_free(player plyr){
	free(plyr);
}
