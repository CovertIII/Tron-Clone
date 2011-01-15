typedef struct clienttype * client;

client client_init(ENetHost * client);

void client_disconnect(client clnt);

void client_update(client clnt, double dt);

int client_connection(client clnt);

void client_render(client clnt);

void client_keys(client clnt, unsigned char key);

void client_process_packets(client clnt, ENetEvent * event);

void client_free(client clnt);
