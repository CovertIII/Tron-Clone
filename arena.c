#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <enet/enet.h>
#include "vector2.h"
#include "player.h"
#include "arena.h"

typedef struct arenatype {
	int plyr_nm;
	int AI_nm;
	vector2 bd;
	player *actors;
} arenatype;

arena arena_init(int plyr_nm, int AI, int x_bd, int y_bd){
	arena arna;

	arna = (arena)malloc(sizeof(arenatype));
	if(arna == NULL)
		{return NULL;}

	arna->actors = (player*)malloc(plyr_nm*sizeof(player));
	if(arna->actors == NULL) {
		free(arna);
		return NULL;
	}

	arna->plyr_nm = 0;
	arna->AI_nm = AI;
	arna->bd.x = x_bd;
	arna->bd.y = y_bd;
	
	int i;
	for(i=0; i<plyr_nm; i++){

		float dir = 2*M_PI*(float)i/((float)plyr_nm);

		arna->actors[i] = player_init((x_bd/2.0f-20)*cos(dir)+x_bd/2.0f,(y_bd/2.0f-20)*sin(dir)+y_bd/2.0f, dir+M_PI);
		if(arna->actors[i]==NULL){
			/*free the other data*/
			int k;
			for (k=0; k < arna->plyr_nm; k++){
				player_free(arna->actors[k]);
			}
			free(arna->actors);
			free(arna);
			return NULL;
		}
		arna->plyr_nm++;
	}
	return arna;
}

int arena_player_status(arena arna){
	int i;
	int num = 0;
	if(arna->plyr_nm == 1)
	{num++;}
	for(i=0; i < arna->plyr_nm; i++){
		if(!player_status(arna->actors[i]))
			{num++;}
	}
	return num;
}

int arena_winner(arena arna){
	int i;
	for(i=0; i < arna->plyr_nm; i++){
		if(!player_status(arna->actors[i]))
			{return i;}
	}
}



void arena_update(arena arna, double dt){
	int i;
	for(i=0; i<arna->plyr_nm; i++){
		player_update(arna->actors[i], dt);
		
		
		if(player_ck_bd(arna->actors[i], arna->bd.x, arna->bd.y)
			|| player_ck_self(arna->actors[i])){
			player_die(arna->actors[i]);
		}
		
		int k;
		for(k=0; k<arna->plyr_nm; k++){
			if(k != i && player_ck_plyr(arna->actors[i], arna->actors[k]) == 1){
				player_die(arna->actors[i]);
			}
			if(k != i && player_ck_plyr(arna->actors[i], arna->actors[k]) == 2){
				player_die(arna->actors[i]);
				player_die(arna->actors[k]);
			}			
		}
	}
}

void arena_update_client(arena arna, double dt){
	int i;
	for(i=0; i<arna->plyr_nm; i++){
		player_update(arna->actors[i], dt);
		player_ck_bd(arna->actors[i], arna->bd.x, arna->bd.y);
	}
}
void arena_render(arena arna){
	glPushMatrix();
	double t =  0.5f * (glutGet(GLUT_WINDOW_WIDTH)/(double)glutGet(GLUT_WINDOW_HEIGHT) * arna->bd.y - arna->bd.x);
	glTranslatef(t + arna->bd.x*0.1, arna->bd.y*0.1, 0);
	int i;
	for (i=0; i < arna->plyr_nm; i++){
		glColor3f((float)i/(float)arna->plyr_nm,0.2,1-(float)i/(float)arna->plyr_nm);
		player_render(arna->actors[i]);
	}
	glLineWidth(3.0);
	glColor3f(1,1,1);	
	glBegin(GL_LINE_LOOP);
		glVertex2f(0,0);
		glVertex2f(0, arna->bd.y);
		glVertex2f(arna->bd.x, arna->bd.y);
		glVertex2f(arna->bd.x, 0);
	glEnd();
	glPopMatrix();
}

int arena_ply_turn(arena arna, int plyr_id, int dir){
	if(arna->plyr_nm > plyr_id){
		player_turn(arna->actors[plyr_id], dir);
	}
}

int arena_ply_speed(arena arna, int plyr_id, int a){
	if(arna->plyr_nm > plyr_id){
		player_speed(arna->actors[plyr_id], a);
	}
}

int arena_plyr_tg(arena arna, int plyr_id){
	if(arna->plyr_nm > plyr_id){
		player_toggle(arna->actors[plyr_id]);
	}
}

void arena_plyr_t_on(arena arna, int plyr_id){
	if(arna->plyr_nm > plyr_id){
		player_trail_on(arna->actors[plyr_id]);
	}
}

void arena_plyr_t_off(arena arna, int plyr_id){
	if(arna->plyr_nm > plyr_id){
		player_trail_off(arna->actors[plyr_id]);
	}
}

void arena_free(arena arna){
	int i;
	for (i=0; i<arna->plyr_nm; i++){
		player_free(arna->actors[i]);
	}
	
	free(arna->actors);
	free(arna);
}


void arena_send_update(arena arna, ENetHost * enet_server, int channel){
	//cycle through the players and send them
	int i;
	for (i = 0; i < arna->plyr_nm; i++){
		player_send_update(arna->actors[i], i, enet_server, channel);	
	}
}


void arena_get_update(arena arna, ENetPacket * packet){
	player_get_update(arna->actors, packet);
}

