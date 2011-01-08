#define NOT_READY 0
#define READY 1
#define PLAYING 2
#define WAITING 3

struct usertype *user;

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
int user_add(user usr, enet_peer *peer, int status);

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
void user_set_ready(user usr, enet_peer *peer);

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

/* user_remove:
 * entry: takes the user list,
 *        takes the enet_peet to remove
 * exit:  removes the user from the list
 *        returns his assigned arena id to kill him
 */
int user_remove(user usr, enet_peer *peer);

/* user_free:
 * entry: takes the user list to free,
 * exit:  deletes list
 */
void user_free(user usr);
