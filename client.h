typedef struct clienttype * client;

client client_init(ENetHost * client);

void client_disconnect(client clnt);

void client_update(client clnt, double dt);

int client_connection(client clnt);

void client_render(client clnt);

void client_keys(client clnt, unsigned char key);

void client_rkeys(client clnt, unsigned char key);

void client_skeys(client clnt, int key);

void client_rskeys(client clnt, int key);

void client_process_packets(client clnt, ENetEvent * event);

vector2 clnt_gm_size(client clnt);

void client_free(client clnt);
