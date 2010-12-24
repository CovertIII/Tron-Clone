#include <math.h>
#include <GLUT/glut.h>

#include "vector2.h"
#include "plist.h"
#include "traillist.h"

typedef struct{
	plist draw_trail;
	plist col_trail;
	int timer;
} trailtype;

typedef struct trailnode {
	trailtype t;
	struct trailnode * next;
} trailnode;

typedef struct {
	trailnode* firsttrail;
	int trailtoggle;
} traillisttype;

trailnode* add_trail(void);

traillist traillist_init(void){
	traillist tlist;

	tlist = (tlist*)malloc(sizeof(traillisttype));

	if (tlist == NULL)
		{return NULL;}

	tlist->trailtoggle = 1;
	tlist->firsttrail = NULL;

	return tlist;
}

void trail_update(traillist tlist, vector2 p, float dt){
	if(tlist->trailtoggle == 1 && tlist->firsttrail = NULL){
		tlist->firsttrail = add_trail(void);

	}
}

trailnode* add_trail(void){
	trailnode *trail;

	trail = (trail*)malloc(sizeof(trailnode));
	if(trail == NULL)
		{return 0;}
	trail->t.timer = 0;
	trail->t.draw_trail = NULL;
	trail->t.col_trail = NULL;
	trail->next = NULL;
	
	return trail;
}
