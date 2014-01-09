/*
 ============================================================================
 Author      : Alejandro Santos, ale@ralo.com.ar
 Version     : 1.0
 ============================================================================
 * 
 * Copyright (C) Alejandro Santos, 2014, ale@ralo.com.ar.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GROUNDED_H_
#define GROUNDED_H_

/* Macros de constantes */

#define MAP_SIZE_X 20
#define MAP_SIZE_Y 15
#define MAP_MAX_ITEMS 300
#define MAP_MAX_FIREB 5
#define ZSPAWN_LIMIT_SPAWN 40

#define GROUNDED_LIMIT_FPS 25

#define GROUNDED_NUM_IMGS 8
#define IMG_W 32
#define IMG_H 32

#define FPS_DELAY_QUEUE_SIZE 4

#define DELTA_MOVE_PLAYER 8
#define DELTA_MOVE_ZOMBIE 2
#define DELTA_MOVE_FIRE   12

#define HEADING_LEFT 0
#define HEADING_RIGHT 1

/* Tipos de tiles */

#define TILE_EMPTY     ' '
#define TILE_FLOOR     '.'
#define TILE_APPLE     '+'
#define TILE_EXIT      '*'
#define TILE_ZOMBIE    'z'
#define TILE_ELEVATOR1 '!'
#define TILE_ELEVATOR2 '?'
#define TILE_ZSPAWN    '$'

/* tipos de imagenes */

enum IMG_TYPE {IMG_EMPTY=0, IMG_FLOOR, IMG_APPLE,
	IMG_PLAYER, IMG_EXIT, IMG_ELEVATOR, IMG_ZOMBIE, IMG_FIRE};

enum ENT_TYPE {ENT_TYPE_PLAYER, ENT_TYPE_ZOMBIE};

/* tipos */

typedef struct {
	SDL_Rect r;
	Uint8 jumping;
	Uint8 grounded;
	Sint16 jump_pos;
	Sint16 delta_move;
	Uint8 heading;
	Uint8 type;
	Sint32 points;
} grounded_ent_t ;

typedef struct {
	SDL_Rect r;
	Uint8 type;
	Uint8 bajando;
	Uint16 nivel_lim;
	Uint16 nivel;
} grounded_elevator_t ;

typedef struct {
	SDL_Rect r;
	Uint8 type;
	Uint8 dir;
} grounded_object_t ;

typedef struct {
	Uint32 num;
	char tiles[MAP_SIZE_Y][MAP_SIZE_X];

	SDL_Rect apples[MAP_MAX_ITEMS];
	Uint32 apples_count;

	SDL_Rect exits[MAP_MAX_ITEMS];
	Uint32 exits_count;

	grounded_ent_t zombies[MAP_MAX_ITEMS];
	Uint32 zombies_count;

	grounded_elevator_t elevators[MAP_MAX_ITEMS];
	Uint32 elevators_count;

	grounded_ent_t player;

	grounded_object_t fireballs[MAP_MAX_FIREB];
	Uint32 fireballs_count;

	SDL_Rect zspawn[MAP_MAX_ITEMS];
	Uint32 zspawn_count;
} grounded_map_t;

void Grounded_Handle_Events(void);
void Grounded_Init_FPS(void);
void Grounded_Limit_FPS(void);
Uint32 Grounded_TimerCallback(Uint32 interval, void *param);
void Grounded_Handle_TimerEvent(void);
void Grounded_Init(void);
void Grounded_Load_Images(void);
void Grounded_Load_Files(void);

void Grounded_Elevator_Init(grounded_elevator_t *el, int x, int y, int type);
void Grounded_Elevator_Update(grounded_elevator_t *el);

void Grounded_Load_NextMap();
void Grounded_Load_Map(int num);
void Grounded_Draw_Map(void);
void Grounded_Map_Update(void);
int Grounded_Map_OutOfBounds(SDL_Rect *r);

int Grounded_Map_FindMinCollition(SDL_Rect *r, SDL_Rect *minpos);
int Grounded_Map_FindMinCollition_ExceptElevator(SDL_Rect *r, SDL_Rect *minpos, const grounded_elevator_t* el);
int Grounded_Map_FindZombieCollition(SDL_Rect *r);

void Grounded_Ent_Reset(grounded_ent_t *e, enum ENT_TYPE t);
void Grounded_Ent_Move(grounded_ent_t *e, int left, int right);
void Grounded_Ent_Update(grounded_ent_t *e);
void Grounded_Ent_CollectItems(grounded_ent_t* e);
void Grounded_Ent_Jump(grounded_ent_t *e);
void Grounded_Ent_FireBall(grounded_ent_t *e);
void Grounded_Ent_Die(grounded_ent_t *e);
void Grounded_Zombie_Init(grounded_ent_t *e, int x, int y);
void Grounded_Zombie_Remove(grounded_ent_t *e);

void Grounded_Update_Player(void);
void Grounded_Game_Loop(void);
void Grounded_Quit(void);


#endif /* GROUNDED_H_ */
