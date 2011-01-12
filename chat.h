typedef struct chattype * chat;

chat chat_init(void);

int chat_add_message(chat cht, char *name, char message[]);

void chat_render(chat cht, int type);

void chat_free(chat cht);

