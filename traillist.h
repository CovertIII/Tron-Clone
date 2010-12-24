typedef struct traillisttype *traillist;

traillist traillist_init(void);

void traillist_update(traillist tlist, vector2 p, float dt);

int trail_on(traillist tlist);

void trail_off(traillist tlist);

void traillist_render(traillist tlist);
