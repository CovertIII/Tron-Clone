/*
I am going to make a tron game using some of the newer things I have learned about the c programming language.

object: arena
*/


typedef struct arenatype *arena;


/**** arena_init: *****
   On entry: number of players, number of AI, x size, y size
   On exit: returns pointer to arena created on success, NULL pointer on failure.
*/
arena arena_init(int plyr_nm, int AI, int x_bd, int y_bd);


/**** arena_update: *****
 * On Entry: the time increment at which to increase the world.
 * On Exit: updates the world
 */
void arena_update(arena arna, double dt);

void arena_update_client(arena arna, double dt);

/**** arena_render: *****
 * On Entry: nothing
 * On Exit:  renders the world using opengl
 */
void arena_render(arena arna);

/**** arena_player_status ***
 * One Entry: takes an arena
 * on exit: return the number of players alive in the arena
 */
int arena_player_status(arena arna);

/*** arena_ply_trn: *****
 * On Entry: player number that's turning, direction you want player to go
 *           , how hard you want your player to turn (1 or 2)
 * On Exit: returns 0 if player is real, 1 if player is AI or doesn't exist
 */
int arena_ply_turn(arena arna, int plyr_id, int dir);

int arena_ply_speed(arena arna, int plyr_id, int a);

/**** arena_ply_tg: ****
 * On Entry: player id to toggle 
 * On Exit: toggles between breaking a trail or starting a new one 
 *          if there is not trail.
 */
int arena_plyr_tg(arena arna, int plyr_id);

/**** arena_free: *****
 * On Entry: arna is and intialised arna
 * On Exit: frees up all the memory used by an arena
 */
void arena_free(arena arna);

void arena_send_update(arena arna, ENetHost * enet_server, int channel);

void arena_get_update(arena arna, ENetPacket * packet);

