typedef struct traillisttype *traillist;

traillist traillist_init(void);

void traillist_update(traillist tlist, vector2 p, float dt);

int trail_on(traillist tlist);

void trail_off(traillist tlist);

void traillist_render(traillist tlist);

int traillist_intersect(traillist tlist, vector2 g1, vector2 g2);

void trail_free(traillist tlist);
