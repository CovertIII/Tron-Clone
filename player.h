/*This is going to control a player object*/

typedef struct playertype *player;

/**** player_init: ****
 * On Entry: x and y coordinates of starting position, dir direction of 
 *           player to point
 * On Exit: returns the pointer to the player
 */
player player_init(int x, int y, double th);

/**** player_update: ****/
void player_update(player plyr, double dt);

/**** player_die: ****/
void player_die(player plyr);

/**** player_turn: ****/
void player_turn(player plyr, int dir);

/**** player_toggle: ****/
void player_toggle(player plyr);

/**** player_render: ****/
void player_render(player plyr);

/**** player_free: ****/
void player_free(player plyr);
