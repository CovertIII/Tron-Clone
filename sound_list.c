#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "vector2.h"
#include "sound_list.h"

typedef struct s_node {
	ALuint src;
	struct s_node * next;
} s_node;

typedef struct s_type {
	s_node * head;
	ALuint buf;
} s_type;

s_list s_init(ALuint buffer){
	s_list sl;
	sl = (s_type*)malloc(sizeof(s_type));
	if(!sl){return NULL;}

	sl->head = NULL;
	sl->buf = buffer;

	return sl;
}


void s_add_snd(s_list sl, vector2 p){
	s_node * new;
	new = (s_node*)malloc(sizeof(s_node));
	if(!new){return;}

	alGenSources(1, &new->src);
	alSourcei(new->src, AL_BUFFER, sl->buf);
	alSource3f(new->src, AL_POSITION, p.x, p.y, 0);
	alSourcePlay(new->src);

	new->next = NULL;

	if(sl->head == NULL){
		sl->head = new;
		return;
	}

	new->next = sl->head;
	sl->head = new;
	return;
}

void s_update(s_list sl){
	s_node * cycle = sl->head;
	s_node * prev = NULL;
	while(cycle != NULL){
		int state;
		alGetSourcei(cycle->src, AL_SOURCE_STATE, &state);
		if(state == AL_STOPPED){
			if(prev == NULL){
				s_node * tmp = cycle->next;
				sl->head = tmp;
				alDeleteSources(1, &cycle->src);
				free(cycle);
				cycle = tmp;
			}
			else{
				prev->next = cycle->next;
				alDeleteSources(1, &cycle->src);
				free(cycle);
				cycle = prev->next;
			}
		}
		else{cycle = cycle->next;}
	}
}

void s_free(s_list sl){
	s_node * cycle = sl->head;
	while(cycle != NULL){
		s_node * tmp = cycle->next;
		alDeleteSources(1, &cycle->src);
		free(cycle);
		cycle = tmp;
	}
}

