typedef struct servertype *server;

server server_init(ENetHost * enet_server);

/* Maybe send something to let the client know a new user joined in this funtion?
 * Also send the state of the server and or arena to the newly connected client
 */
void server_add_user(server svr, ENetPeer *peer); 

void server_remove_user(server svr, ENetPeer *peer); //send somethign to lset clients know a user has disconnected

void server_update(server svr, double dt);

void server_send_updates(server svr);

void server_process_packet(server svr, ENetEvent event);

void server_free(server svr);

