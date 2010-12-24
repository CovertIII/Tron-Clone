#include "vector2.h"
#include "plist.h"
#include "traillist.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>


typedef struct{
	plist draw_trail;
	plist col_trail;
} trailtype;

typedef struct trailnode {
	trailtype t;
	struct trailnode * next;
} trailnode;

typedef struct traillisttype {
	trailnode* firsttrail;
	int trailtoggle;
	float col_timer;
} traillisttype;

trailnode* add_trail(void);

traillist traillist_init(void){
	traillist tlist;

	tlist = (traillist)malloc(sizeof(traillisttype));

	if (tlist == NULL)
		{return NULL;}

	tlist->trailtoggle = 1;
	tlist->col_timer = 0;
	tlist->firsttrail = NULL;

	return tlist;
}

void traillist_update(traillist tlist, vector2 p, float dt){
	tlist->col_timer += dt;
	if(tlist->trailtoggle == 1 && tlist->firsttrail == NULL){
		tlist->firsttrail = add_trail();
		plist_add(&tlist->firsttrail->t.draw_trail, p);
		plist_add(&tlist->firsttrail->t.col_trail, p);
	}


	trailnode* trail = tlist->firsttrail;

	if(tlist->trailtoggle){
		plist_add(&trail->t.draw_trail, p);
		if(tlist->col_timer > 0.1){
			plist_add(&trail->t.col_trail, p);
			tlist->col_timer = 0;
		}
//		trail = trail->next;
	}	

	/*TODO once you find out that a trail is done,
	 * free the trailnode and reoranize the list.
	 * */
	while(trail != NULL){
		plist_update(trail->t.draw_trail, dt);
		plist_update(trail->t.col_trail, dt);
		trail = trail->next;
	}
}

int trail_on(traillist tlist){
	if(tlist->trailtoggle == 1)
		{return 0;}

	tlist->trailtoggle = 1;
	trailnode *newtrail = add_trail();
	newtrail->next = tlist->firsttrail;
	tlist->firsttrail = newtrail;
	return 0;
}

void trail_off(traillist tlist){
	tlist->trailtoggle = 0;
}

void traillist_render(traillist tlist){
	trailnode *cycle = tlist->firsttrail;
	while(cycle != NULL){
		plist_render(cycle->t.draw_trail);
		cycle = cycle->next;
	}
}

trailnode* add_trail(void){
	trailnode *trail;

	trail = (trailnode*)malloc(sizeof(trailnode));
	if(trail == NULL)
		{return 0;}
	trail->t.draw_trail = NULL;
	trail->t.col_trail = NULL;
	trail->next = NULL;
	
	return trail;
}
