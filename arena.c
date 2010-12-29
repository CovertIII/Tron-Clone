#include "arena.h"
#include "player.h"
#include "vector2.h"
#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>

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
	for(i=0; i < arna->plyr_nm; i++){
		if(!player_status(arna->actors[i]))
			{num++;}
	}
	return num;
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
			if(
				k != i && 
				player_ck_plyr(arna->actors[i], arna->actors[k])
			){
				player_die(arna->actors[i]);
			}
		}
	}
}

void arena_render(arena arna){
	int i;
	for (i=0; i < arna->plyr_nm; i++){
		glColor3f((float)i/(float)arna->plyr_nm, 0.5, (float)i/(float)arna->plyr_nm);
		player_render(arna->actors[i]);
	}
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

void arena_free(arena arna){
	int i;
	for (i=0; i<arna->plyr_nm; i++){
		player_free(arna->actors[i]);
	}
	
	free(arna->actors);
	free(arna);
}
