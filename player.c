#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "vector2.h"
#include "math.h"
#include "traillist.h"

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
	int trailtoggle;	
	traillist trails;	
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
	
	plyr->trailtoggle = 1;
	plyr->trails = traillist_init();	
	
	return plyr;
}

void player_update(player plyr, double dt){
	plyr->th += plyr->w*dt;
	plyr->vel += plyr->a*dt;

	plyr->v.x = plyr->vel * cos(plyr->th);
	plyr->v.y = plyr->vel * sin(plyr->th);

	plyr->p.x += plyr->v.x*dt;
	plyr->p.y += plyr->v.y*dt;
	
	
	vector2 tmp = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	
	traillist_update(plyr->trails, tmp, dt);	
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

void player_toggle(player plyr){
	if(plyr->trailtoggle ==1){
		plyr->trailtoggle = 0;
		trail_off(plyr->trails);
	}
	else{
		plyr->trailtoggle = 1;
		trail_on(plyr->trails);
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
	traillist_render(plyr->trails);
	glPopMatrix();
}

void player_free(player plyr){
	free(plyr);
}
