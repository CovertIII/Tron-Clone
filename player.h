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

/**** player_status (returns 0 if player is alive, and one if he is dead) ***/
int player_status(player plyr);

/*** player_ck_bd: ***/
int player_ck_bd(player plyr, int x, int y);

/*** player_ck_self: ***/
int player_ck_self(player plyr);

/** player_ck_plyr: ***/
int player_ck_plyr(player plyr_hd, player plyr_tl);

/**** player_turn: ****/
void player_turn(player plyr, int dir);

/**** player_change_speed: ****/
void player_speed(player plyr, int a);

/**** player_toggle: ****/
void player_toggle(player plyr);

/**** player_render: ****/
void player_render(player plyr);

/**** player_free: ****/
void player_free(player plyr);
