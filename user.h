#define NOT_READY 0
#define READY 1
#define PLAYING 2
#define WAITING 3

typedef struct usertype *user;

//General Managment of the list
/*user_init
 * Entry: Nothing
 * Exit: gives address of user
 */
user user_init(void);

/* user_add: (call when ever a user joins,
 *    set status to NOT_READY if server state is LOBBY, 
 *    and WAITING if a server state is GAME)
 * entry: enet_peer of the user, status of the user
 * exit: add the user to the user list and 
 *       set the user's score to zero
 */
int user_add(user usr, ENetPeer *peer, int status);

/* user_remove:
 * entry: takes the user list,
 *        takes the enet_peet to remove
 * exit:  removes the user from the list
 *        returns his assigned arena id to kill him
 */
int user_remove(user usr, ENetPeer *peer);

void user_render(user usr, int x, int y);

/* user_free:
 * entry: takes the user list to free,
 * exit:  deletes list
 */
void user_free(user usr);


//These functions are for the game logic

/* user_update_score: (call at the end of an arena game)
 * entry: takes the user list,
 *        takes the score for the certain round,
 *        takes the arena id of the person.
 * exit: adds the score to the user's total score,
 *       sets everyone to not ready
 */
void user_update_score(user usr, int arna_id ,int score);

/* user_set_ready:
 * entry: takes user list,
 *        takes enet_peer to set to ready
 * exit:  sets the specified user to a ready state
 */
void user_set_ready(user usr, ENetPeer *peer);

/* returns the numbers of users on the list*/
int user_number(user usr);

/* user_check_states:
 * entry: takes user list
 * exit:  returns number of NOT_READY users
 */
int user_check_states(user usr);

/* user_set_arena_id:
 * entry: takes the user list
 * exit:  sets an arena id for each ready member,
 *        returns number of ids set,
 *        also sets each READY player to PLAYING
 */
int user_set_arena_id(user usr);

/* user_all_not_ready:
 * set the state of all users in usr to not ready
 */
void user_all_not_ready(user usr);

//These functions are for send and recieving packets

/* on connection the server gets a new client, this is w
 * where the server sends the info about all clients that have joined
 */
void user_send_list(user usr, ENetHost * host, int channel);

/* for the client recieving the list */
/* this is what gets updates about the list too:
 *   call this function in the client to after calling these functions in the server:
 *    o user_send_list
 *    o user_send_new_client
 */
void user_get_list(user usr, ENetPacket * packet);

/* when a client changes their name as it appears on the server */
/* for the server: usr is user list to modify, packet is the packet recived by the server with a string of the new name, channel is the new channel to send the name change out to.*/
void user_change_name_send(user usr, ENetHost * server,  ENetEvent * event, enet_uint8 channel);

/* when a new user joins to send his userinfo to the rest of the clients*/
void user_send_new_client(user usr, ENetPeer * peer, ENetHost *host, enet_uint8 channel);

/* when a client disconnects, notify the other users that a user disconnected */
void user_send_disconnect(int id, int channel, ENetHost * host);

/* for the client */
void user_get_disconnect(user usr, ENetPacket * packet);

void user_send_chat_message(user usr, ENetEvent * event, ENetHost * host, enet_uint8 channel);

void user_get_chat_message(user usr, chat cht, ENetPacket *packet);

void user_sget_send_pstate(user usr, ENetHost * server,  ENetEvent * event, enet_uint8 channel);

int user_peer_aid(user usr, ENetPeer * peer);

void user_send_arena_ids(user usr, int channel);

int user_get_arena_id(ENetPacket * packet);

