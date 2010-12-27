#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "vector2.h"
#include "math.h"
#include "traillist.h"
#include "collison.h"

#define VELOCITY 100
#define MIN_VEL 75
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
	int dead;
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
	plyr->v.x = VELOCITY*cos(th);
	plyr->v.y = VELOCITY*sin(th);
	plyr->length = LENGTH;
	
	plyr->dead = 0;
	plyr->trailtoggle = 1;
	plyr->trails = traillist_init();	
	
	return plyr;
}

void player_update(player plyr, double dt){
	plyr->th += plyr->w*dt;
	plyr->vel += plyr->a*dt;

	plyr->v.x += (3*plyr->vel * cos(plyr->th) - 3*plyr->v.x)*dt;
	plyr->v.y += (3*plyr->vel * sin(plyr->th) - 3*plyr->v.y)*dt;

	plyr->p.x += plyr->v.x*dt;
	plyr->p.y += plyr->v.y*dt;
	
	
	vector2 tmp = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	
	traillist_update(plyr->trails, tmp, dt);	
}

int player_ck_bd(player plyr, int x, int y){
	vector2 c1 = {0,0};
	vector2 c2 = {0,y};
	vector2 c3 = {x,y};
	vector2 c4 = {x,0};

	vector2 t1 = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	vector2 t2 = { plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y};

	if(line_collison(c1, c2, t1, t2) || line_collison(c2, c3, t1, t2) ||
		line_collison(c3, c4, t1, t2) || line_collison(c4, c1, t1, t2)){
		plyr->trailtoggle = 0;
		trail_off(plyr->trails);
		return 1;
	}
	return 0;
}

int player_ck_self(player plyr){
	
	vector2 t1 = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	vector2 t2 = { plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y};
	
	if(traillist_intersect(plyr->trails, t1, t2)){
		plyr->trailtoggle = 0;
		trail_off(plyr->trails);
		return 1;
	}
	return 0;
}

int player_ck_plyr(player plyr_hd, player plyr_tl){
	
	vector2 hd1 = {-plyr_hd->length*cos(plyr_hd->th)+plyr_hd->p.x, -plyr_hd->length*sin(plyr_hd->th)+plyr_hd->p.y};
	vector2 hd2 = { plyr_hd->length*cos(plyr_hd->th)+plyr_hd->p.x,  plyr_hd->length*sin(plyr_hd->th)+plyr_hd->p.y};
	vector2 tl1 = {-plyr_tl->length*cos(plyr_tl->th)+plyr_tl->p.x, -plyr_tl->length*sin(plyr_tl->th)+plyr_tl->p.y};
	vector2 tl2 = { plyr_tl->length*cos(plyr_tl->th)+plyr_tl->p.x,  plyr_tl->length*sin(plyr_tl->th)+plyr_tl->p.y};
	
	if(traillist_intersect(plyr_tl->trails, hd1, hd2) || line_collison(hd1, hd2, tl1, tl2)){
		plyr_hd->trailtoggle = 0;
		trail_off(plyr_hd->trails);
		return 1;
	}
	return 0;
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

void player_die(player plyr){
	plyr->dead = 1;
}

void player_free(player plyr){
	free(plyr);
}
