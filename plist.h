typedef struct pnode *plist;

int plist_add(plist *list, vector2 p);

int plist_update(plist list, float dt);

void plist_render(plist list);
