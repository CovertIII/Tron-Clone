#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "vector2.h"
#include "math.h"
#include "plist.h"

#define VELOCITY 100
#define MIN_VEL 30
#define MAX_VEL 200
#define LENGTH 15

typedef struct playertype {
	vector2 p;
	vector2 v;
	float vel;
	float a;
	float th;
	float w;
	float length;
	
	float timer;
	
	plist trail;
	plist col_trail;
	
} playertype;

player player_init(int x, int y, double th){
	player plyr;

	plyr = (player)malloc(sizeof(playertype));
	if(plyr == NULL)
		{return NULL;}

	plyr->p.x = x;
	plyr->p.y = y;
	plyr->vel = VELOCITY;
	plyr->a =0;
	plyr->w = 0;
	plyr->th = th;
	plyr->v.x = -VELOCITY*cos(th);
	plyr->v.y = VELOCITY*sin(th);
	plyr->length = LENGTH;
	
	plyr->timer = 0;
	
	plyr->trail = NULL;
	plyr->col_trail = NULL;
	
	vector2 tmp = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	
	plist_add(&plyr->col_trail, tmp);
	
	return plyr;
}

void player_update(player plyr, double dt){
	plyr->th += plyr->w*dt;
	plyr->vel += plyr->a*dt;

	plyr->v.x = plyr->vel * cos(plyr->th);
	plyr->v.y = plyr->vel * sin(plyr->th);

	plyr->p.x += plyr->v.x*dt;
	plyr->p.y += plyr->v.y*dt;
	
	plyr->timer += dt;
	
	vector2 tmp = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	
	plist_update(plyr->trail, dt);
	plist_add(&plyr->trail, tmp);
	
	plist_update(plyr->col_trail, dt);
	if (plyr->timer > 0.1){
		plist_add(&plyr->col_trail, tmp);
		plyr->timer = 0;
	}
}

void player_turn(player plyr, int dir){
	plyr->w = dir;
}

void player_speed(player plyr, int a){
	plyr->a = a;

	if ((plyr->vel > MAX_VEL && a > 0) || (plyr->vel < MIN_VEL && a < 0)){
		plyr->a = 0;
	}

}

void player_render(player plyr){
	glPushMatrix();
	glLineWidth(3.0);
	
	glBegin(GL_LINES);
		glVertex2f(plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y);
		glVertex2f(-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y);
	glEnd();
	
	glLineWidth(2.0);
	plist_render(plyr->trail);
	//plist_render(plyr->col_trail);
	
	glPopMatrix();
}

void player_free(player plyr){
	free(plyr);
}
