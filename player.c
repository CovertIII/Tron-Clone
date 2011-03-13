#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <enet/enet.h>
#include <tpl.h>

#include "vector2.h"
#include "sound_list.h"
#include "player.h"
#include "math.h"
#include "traillist.h"
#include "collison.h"
#include "particle_sys.h"

#define VELOCITY 100
#define MIN_VEL 75
#define MAX_VEL 200
#define LENGTH 15

typedef struct playertype {
	vector2 p;
	vector2 v;
	double vel;
	double a;
	double th;
	double w;
	double length;
	int trailtoggle;	
	int dead;
	traillist trails;	
	particles ghost;
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
	plyr-> ghost = NULL;
	plyr->trailtoggle = 1;
	plyr->trails = traillist_init();	
	
	return plyr;
}

int player_status(player plyr){
	return plyr->dead;
}

vector2 player_pos(player plyr){
	return plyr->p; 
}

vector2 player_vel(player plyr){
	return plyr->v;
}

void player_update(player plyr, double dt){
	if(plyr->dead == 0){
		plyr->th += plyr->w*dt;
		plyr->vel += plyr->a*dt;
	
		plyr->v.x += (3*plyr->vel * cos(plyr->th) - 3*plyr->v.x)*dt;
		plyr->v.y += (3*plyr->vel * sin(plyr->th) - 3*plyr->v.y)*dt;
	
		plyr->p.x += plyr->v.x*dt;
		plyr->p.y += plyr->v.y*dt;
	}	

	particles_update(plyr->ghost, dt);
	
	vector2 tmp = {-1*(plyr->length+1)*cos(plyr->th)+plyr->p.x, -1*(plyr->length+1)*sin(plyr->th)+plyr->p.y};
	
	traillist_update(plyr->trails, tmp, dt);	
}

int player_ck_bd(player plyr, int x, int y){
	vector2 c1 = {0,0};
	vector2 c2 = {0,y};
	vector2 c3 = {x,y};
	vector2 c4 = {x,0};

	particles_bound(plyr->ghost, x, y);

	if(plyr->dead)
		{return 0;}

	vector2 t1 = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	vector2 t2 = { plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y};

	if(line_collison(c1, c2, t1, t2) || line_collison(c2, c3, t1, t2) ||
		line_collison(c3, c4, t1, t2) || line_collison(c4, c1, t1, t2)){
		return 1;
	}
	return 0;
}

int player_ck_self(player plyr){
	
	if(plyr->dead)
		{return 0;}

	vector2 t1 = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	vector2 t2 = { plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y};
	//There is a bug in checking the trails.  It appears as if you can go through the last segment of any trail.  That
	//is because when you turn a trail off it does not update the collision trail to add a point to where you turned it
	//off.  Therefore you can go through part of the draw trail, which is updated every frame. 	
	if(traillist_intersect(plyr->trails, t1, t2)){
		return 1;
	}
	return 0;
}

int player_ck_plyr(player plyr_hd, player plyr_tl){
	
	if(plyr_hd->dead && plyr_tl->dead)
		{return 0;}

	vector2 hd1 = {-plyr_hd->length*cos(plyr_hd->th)+plyr_hd->p.x, -plyr_hd->length*sin(plyr_hd->th)+plyr_hd->p.y};
	vector2 hd2 = { plyr_hd->length*cos(plyr_hd->th)+plyr_hd->p.x,  plyr_hd->length*sin(plyr_hd->th)+plyr_hd->p.y};
	vector2 tl1 = {-plyr_tl->length*cos(plyr_tl->th)+plyr_tl->p.x, -plyr_tl->length*sin(plyr_tl->th)+plyr_tl->p.y};
	vector2 tl2 = { plyr_tl->length*cos(plyr_tl->th)+plyr_tl->p.x,  plyr_tl->length*sin(plyr_tl->th)+plyr_tl->p.y};
	
	if(traillist_intersect(plyr_tl->trails, hd1, hd2)){
		return 1;
	}
	if(line_collison(hd1, hd2, tl1, tl2) && plyr_tl->dead == 0 && plyr_hd->dead == 0){
		return 2;
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
	else if (plyr->trailtoggle == 0 && plyr->dead == 0){
		plyr->trailtoggle = 1;
		trail_on(plyr->trails);
	}
	else
	{
		plyr->trailtoggle = 0;
		trail_off(plyr->trails);
	}
}

void player_trail_on(player plyr){
	if(plyr->trailtoggle == 1){
		return;
	}
	plyr->trailtoggle = 1;
	trail_on(plyr->trails);
}	

void player_trail_off(player plyr){
	if(plyr->trailtoggle == 0){
		return;
	}
	plyr->trailtoggle = 0;
	trail_off(plyr->trails);
}


void player_render(player plyr){
	glPushMatrix();
	particles_render(plyr->ghost);
	if(plyr->dead == 0){
		glLineWidth(3.0);
	
		glBegin(GL_LINES);
			glVertex2f(plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y);
			glVertex2f(-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y);
		glEnd();
	}	

	glLineWidth(1.5);
	traillist_render(plyr->trails);
	glPopMatrix();
}

void player_die(player plyr){
	if(plyr->dead)
		{return;}
	plyr->dead = 1;
	plyr->trailtoggle = 0;
	trail_off(plyr->trails);
	vector2 tmp1 = {-plyr->length*cos(plyr->th)+plyr->p.x, -plyr->length*sin(plyr->th)+plyr->p.y};
	vector2 tmp2 = { plyr->length*cos(plyr->th)+plyr->p.x, plyr->length*sin(plyr->th)+plyr->p.y};

	plyr->ghost = particles_init(tmp1, tmp2, plyr->v);
}

void player_free(player plyr){
	particles_free(plyr->ghost);
	trail_free(plyr->trails);
	free(plyr);
}

void player_send_update(player plyr, int plyr_id, ENetHost * host, int channel){
	playertype splyr = *plyr;
	tpl_node *tn;
	void *addr;
	size_t len;

	tn = tpl_map("iS($(ff)$(ff)fffffii)", &plyr_id, &splyr);
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	
	ENetPacket * packet;
	packet = enet_packet_create (addr, len, 0);
	enet_host_broadcast (host, channel, packet);
	free(addr);
}

void player_get_update(player *actors, ENetPacket * packet, ALuint * buz, ALuint * eng, s_list death){
	int plyr_id;
	playertype splyr;
	tpl_node * tn;
	tn = tpl_map("iS($(ff)$(ff)fffffii)", &plyr_id, &splyr);
	tpl_load(tn, TPL_MEM, packet->data, packet->dataLength);
	tpl_unpack(tn,0); 
	tpl_free(tn);

	int tt = actors[plyr_id]->trailtoggle;
	int dd = actors[plyr_id]->dead;

	traillist tmp_trail = actors[plyr_id]->trails;
	particles tmp_part = actors[plyr_id]->ghost;

	*actors[plyr_id] = splyr;
	actors[plyr_id]->trails = tmp_trail;
	actors[plyr_id]->ghost = tmp_part;

	if(tt != actors[plyr_id]->trailtoggle){
		actors[plyr_id]->trailtoggle = tt;
		player_toggle(actors[plyr_id]);
		tt ? alSourceStop(buz[plyr_id]) : alSourcePlay(buz[plyr_id]);
	}

	if (dd != actors[plyr_id]->dead){
		alSourceStop(eng[plyr_id]);
		s_add_snd(death, actors[plyr_id]->p);
		vector2 tmp1 = {-actors[plyr_id]->length*cos(actors[plyr_id]->th)+actors[plyr_id]->p.x, -actors[plyr_id]->length*sin(actors[plyr_id]->th)+actors[plyr_id]->p.y};
		vector2 tmp2 = { actors[plyr_id]->length*cos(actors[plyr_id]->th)+actors[plyr_id]->p.x, actors[plyr_id]->length*sin(actors[plyr_id]->th)+actors[plyr_id]->p.y};
		actors[plyr_id]->ghost = particles_init(tmp1, tmp2, actors[plyr_id]->v);
	}

}

