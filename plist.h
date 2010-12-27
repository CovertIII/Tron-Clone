typedef struct pnode *plist;

int plist_add(plist *list, vector2 p);

int plist_update(plist list, float dt);

void plist_render(plist list);

int plist_intersect(plist list, vector2 g1, vector2 g2);
