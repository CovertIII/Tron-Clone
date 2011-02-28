typedef struct s_type * s_list;

s_list s_init(ALuint buffer);

void s_add_snd(s_list snd, vector2 p);

void s_update(s_list snd);

void s_free(s_list snd);
