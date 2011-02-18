typedef struct particlenode *particles;

particles particles_init(vector2 pt1, vector2 pt2, vector2 v);

void particles_update(particles pts, float dt);

void particles_bound(particles pts, int x, int y);

void particles_render(particles pts);

void particles_free(particles pts);
