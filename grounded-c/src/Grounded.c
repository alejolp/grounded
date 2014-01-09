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

#include <SDL/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#include "Util.h"
#include "Grounded.h"


/* Surfaces */

SDL_Surface* screen = 0;
SDL_Surface* imgs = 0;
SDL_Rect imgs_rects[GROUNDED_NUM_IMGS];

/* variables de video. */

const int W = 640;
const int H = 480;
int video_flags = 0;

int run = 1;
Uint8 teclas[SDLK_LAST];


/* variables del juego */

grounded_map_t map_data;

/* Control de FPS */

Uint32 fps_lastcheck = 0;
Uint32 fps_tmp_counter = 0;
Uint32 fps_fps = 0;
Uint32 fps_delay_sum = 0;
Uint32 fps_delay_prev = 0;
Uint32 FPS_ideal_delay = 0;

const int FPS = GROUNDED_LIMIT_FPS * 10;

/* Funciones. */

void Grounded_Handle_Events(void) {
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT) {
			run = 0;
		} else if (ev.type == SDL_KEYDOWN) {
			if (ev.key.keysym.sym == SDLK_ESCAPE) {
				run = 0;
			} else if (ev.key.keysym.sym == SDLK_F1) {
				Grounded_Load_NextMap();
			} else if (ev.key.keysym.sym == SDLK_F5) {
				SDL_WM_ToggleFullScreen(screen);
			} else if (ev.key.keysym.sym == SDLK_SPACE) {
				Grounded_Ent_Jump(&map_data.player);
			} else if (ev.key.keysym.sym == SDLK_z) {
				Grounded_Ent_FireBall(&map_data.player);
			} else {
				teclas[ev.key.keysym.sym] = 1;
			}
		} else if (ev.type == SDL_KEYUP) {
			teclas[ev.key.keysym.sym] = 0;
		} else if (ev.type == SDL_USEREVENT + 0) {
			Grounded_Handle_TimerEvent();
		}
	}
}

void Grounded_Handle_TimerEvent(void) {
	if (map_data.zspawn_count > 0) {
		if ((map_data.apples_count == 0) && (map_data.zombies_count < ZSPAWN_LIMIT_SPAWN)) {
			int i;

			for (i = 0; i < map_data.zspawn_count; ++i) {
				Grounded_Zombie_Init(&map_data.zombies[map_data.zombies_count++],
						map_data.zspawn[i].x, map_data.zspawn[i].y);
			}
		}
	}
}

void Grounded_Init_FPS(void) {
	FPS_ideal_delay = 10000 / FPS;
	fps_lastcheck = SDL_GetTicks();
	fps_delay_prev = FPS_ideal_delay;
	fps_delay_sum = FPS_ideal_delay * FPS_DELAY_QUEUE_SIZE;
}

void Grounded_Limit_FPS(void) {
	/*
	 * El limitador de FPS trabaja con decenas de segundos; eso es,
	 * todos los valores estan multiplicados por 10. De esta forma
	 * no hace falta usar punto flotante.
	 *
	 * 16 FPS => 160.
	 * 1000 mseg => 10000.
	 *
	 */

	Uint32 ct = SDL_GetTicks();
	Uint32 last_delta = ct - fps_lastcheck;

#if 1
	if (fps_fps != 0 && fps_fps != FPS) {
		SDL_Delay(fps_delay_prev);
	} else {
		SDL_Delay(10000 / FPS);
	}
#endif

	if (last_delta > 1000) { /* aca si que hace falta usar 1000. */
		Uint32 new_delay;

		fps_fps = 10000 * fps_tmp_counter / last_delta;

#if 0
		if (fps_fps < FPS && fps_tmp_counter > 0) {
			Uint32 last_delay_per_frame = (last_delta - fps_tmp_counter * fps_delay_prev) / fps_tmp_counter;
			new_delay = (last_delay_per_frame > FPS_ideal_delay) ? 0 : (FPS_ideal_delay - last_delay_per_frame);
		} else {
			new_delay = FPS_ideal_delay;
		}
#else
		if (fps_tmp_counter > 0) {
			Uint32 last_delay_per_frame = (last_delta - fps_tmp_counter * fps_delay_prev) / fps_tmp_counter;
			new_delay = (last_delay_per_frame > FPS_ideal_delay) ? 0 : (FPS_ideal_delay - last_delay_per_frame);
		} else {
			new_delay = FPS_ideal_delay;
		}
#endif

		fps_delay_sum -= fps_delay_prev;
		fps_delay_sum += new_delay;
		fps_delay_prev = fps_delay_sum / FPS_DELAY_QUEUE_SIZE;

		fps_tmp_counter = 0;
		fps_lastcheck = ct;

		{
			char buf[100];
			snprintf(buf, sizeof(buf), "Grounded - %2.1f FPS", fps_fps / 10.0);
			SDL_WM_SetCaption(buf, NULL);
		}
	}

	++fps_tmp_counter;
}

Uint32 Grounded_TimerCallback(Uint32 interval, void *param) {
	/*
	typedef Uint32 (*SDL_NewTimerCallback)(Uint32 interval, void *param);
	*/

	SDL_Event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = SDL_USEREVENT + 0;

	SDL_PushEvent(&ev);

	return interval;
}

void Grounded_Init(void) {

	/* */

	/* Video */

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		Grounded_SDL_Error();

	video_flags = SDL_HWSURFACE | SDL_DOUBLEBUF;

	screen = SDL_SetVideoMode(W, H, 0, video_flags);
	if (screen == NULL)
		Grounded_SDL_Error();

	/* WM */

	SDL_WM_SetCaption("Grounded", NULL);

	/* FPS */

	Grounded_Init_FPS();

	/* Misc */

	SDL_AddTimer(1000, Grounded_TimerCallback, 0);

	memset(teclas, 0, sizeof(teclas));

	unsigned int rand_seed = SDL_ThreadID() ^ SDL_GetTicks();
	srand(rand_seed);

}

void Grounded_Load_Images(void) {
	int i;

	/* Image file */

	imgs = Util_LoadBMP("img/tiles.bmp");

	/* Rects */

	for (i=0; i < GROUNDED_NUM_IMGS; ++i) {
		imgs_rects[i].w = IMG_W;
		imgs_rects[i].h = IMG_H;
		imgs_rects[i].x = IMG_W * i;
		imgs_rects[i].y = 0;
	}
}

void Grounded_Load_NextMap() {
	Grounded_Load_Map(map_data.num + 1);
}

void Grounded_Elevator_Init(grounded_elevator_t *el, int x, int y, int type) {
	el->type = type;
	el->bajando = 1;
	el->nivel_lim = H - y - IMG_H;
	el->nivel = el->nivel_lim;

	Util_Init_Rect(&el->r, x, y, IMG_W, IMG_H);
}

void Grounded_Elevator_Update(grounded_elevator_t *el) {
	if (el->type == 2) {
		SDL_Rect tmprect;

		el->r.y += (el->bajando) ? 1 : (-1);

		if (Grounded_Map_FindMinCollition_ExceptElevator(&el->r, &tmprect, el) || Grounded_Map_OutOfBounds(&el->r)) {
			el->r.y -= (el->bajando) ? 1 : (-1);
			el->bajando = !el->bajando;
		}
	} else {
		el->r.y += (el->bajando) ? 1 : (-1);
		el->nivel--;

		if (el->nivel == 0) {
			el->nivel = el->nivel_lim;
			el->bajando = !el->bajando;
		}
	}
}

void Grounded_Load_Map(int num) {
	FILE* fp;
	int x, y;
	char mapfile[1000];
	char buf[MAP_SIZE_X + 2];

	memset(&map_data, 0, sizeof(map_data));
	snprintf(mapfile, sizeof(mapfile), "maps/%d.txt", num);

	map_data.num = num;
	map_data.apples_count = 0;
	map_data.exits_count = 0;

	fp = fopen(mapfile, "rb");
	if (fp == NULL)
		Grounded_Std_Error();

	for (y=0; y<MAP_SIZE_Y; ++y) {
		if (NULL == fgets(buf, sizeof(buf) / sizeof(buf[0]), fp))
			Grounded_Std_Error();

		for (x=0; x<MAP_SIZE_X && buf[x]; ++x) {
			map_data.tiles[y][x] = buf[x];

			switch (buf[x]) {
			case TILE_APPLE:
				Util_Init_Rect(&map_data.apples[map_data.apples_count],
						x * IMG_W, y * IMG_H,
						IMG_W, IMG_H);
				map_data.apples_count++;
				break;

			case TILE_ELEVATOR1:
				Grounded_Elevator_Init(&map_data.elevators[map_data.elevators_count++], x * IMG_W, y * IMG_H, 1);
				break;

			case TILE_ELEVATOR2:
				Grounded_Elevator_Init(&map_data.elevators[map_data.elevators_count++], x * IMG_W, y * IMG_H, 2);
				break;

			case TILE_EMPTY:
				break;

			case TILE_EXIT:
				Util_Init_Rect(&map_data.exits[map_data.exits_count],
						x * IMG_W, y * IMG_H,
						IMG_W, IMG_H);
				map_data.exits_count++;
				break;

			case TILE_FLOOR:
				break;

			case TILE_ZOMBIE:
				Grounded_Zombie_Init(&map_data.zombies[map_data.zombies_count++], x * IMG_W, y * IMG_H);
				break;

			case TILE_ZSPAWN:
				Util_Init_Rect(&map_data.zspawn[map_data.zspawn_count],
						x * IMG_W, y * IMG_H,
						IMG_W, IMG_H);
				map_data.zspawn_count++;
				break;

			default:
				break;
			}
		}

		if (x < MAP_SIZE_X)
			Grounded_Error("Mapa invalido");
	}

	fclose(fp);

	/* Player  */

	Grounded_Ent_Reset(&map_data.player, ENT_TYPE_PLAYER);

}

void Grounded_Load_Files(void) {
	Grounded_Load_Images();
	Grounded_Load_Map(1);
}

void Grounded_Draw_Map(void) {
	int x, y, num;
	SDL_Rect dst;

	dst.w = IMG_W;
	dst.h = IMG_H;

	for (y=0; y < MAP_SIZE_Y; ++y) {
		for (x=0; x < MAP_SIZE_X; ++x) {
			dst.x = x * IMG_W;
			dst.y = y * IMG_H;

			/* Layer 0 */

			switch (map_data.tiles[y][x]) {
			case TILE_FLOOR:
				num = IMG_FLOOR;
				break;

			case TILE_EMPTY:
			default:
				num = IMG_EMPTY;
				break;
			}

			SDL_BlitSurface(imgs, &imgs_rects[num],
					screen, &dst);

			/* Layer 1 */

			switch (map_data.tiles[y][x]) {
			case TILE_APPLE:
			case TILE_ZSPAWN:
				num = IMG_APPLE;
				break;

			case TILE_EXIT:
				num = IMG_EXIT;
				break;

			default:
				num = -1;
			}

			if (num != -1) {
				SDL_BlitSurface(imgs, &imgs_rects[num],
						screen, &dst);
			}

		}
	}

#if 0
	/* manzanas */

	for(x=0; x<map_data.apples_count; ++x) {
		SDL_BlitSurface(imgs, &imgs_rects[IMG_APPLE],
				screen, &map_data.apples[x]);
	}

	/* exits */

	for(x=0; x<map_data.exits_count; ++x) {
		SDL_BlitSurface(imgs, &imgs_rects[IMG_EXIT],
				screen, &map_data.exits[x]);
	}
#endif

	/* Elevators */

	for(x=0; x<map_data.elevators_count; ++x) {
		SDL_BlitSurface(imgs, &imgs_rects[IMG_ELEVATOR],
				screen, &map_data.elevators[x].r);
	}

	/* zombies */

	for(x=0; x<map_data.zombies_count; ++x) {
		SDL_BlitSurface(imgs, &imgs_rects[IMG_ZOMBIE],
				screen, &map_data.zombies[x].r);
	}

	/* Fire balls */

	for(x=0; x<map_data.fireballs_count; ++x) {
		SDL_BlitSurface(imgs, &imgs_rects[IMG_FIRE],
				screen, &map_data.fireballs[x].r);
	}

	/* Player */

	SDL_BlitSurface(imgs, &imgs_rects[IMG_PLAYER],
			screen, &map_data.player.r);

}

void Grounded_Map_Update(void) {
	int i;
	SDL_Rect tmprect;

	/* Zombies */

	for (i = 0; i < map_data.zombies_count; ++i) {
		grounded_ent_t *e = &map_data.zombies[i];

		Grounded_Ent_Update(e);

		SDL_Rect oldpos = e->r;

		Grounded_Ent_Move(e, e->heading == HEADING_LEFT, e->heading == HEADING_RIGHT);

		if ((oldpos.x == e->r.x) && (oldpos.y == e->r.y)) {
			e->heading = ((e->heading == HEADING_LEFT) ? HEADING_RIGHT : HEADING_LEFT);
		}
	}

	/* Fireballs */

	for (i = 0; i < map_data.fireballs_count; ++i) {
		grounded_object_t *fb = &map_data.fireballs[i];
		int destroy_ball, zombie_num;

		if (fb->dir == HEADING_LEFT) {
			fb->r.x -= DELTA_MOVE_FIRE;
		} else {
			fb->r.x += DELTA_MOVE_FIRE;
		}

		zombie_num = Grounded_Map_FindZombieCollition(&fb->r);

		if (zombie_num != -1) {
			destroy_ball = 1;
			Grounded_Zombie_Remove(&map_data.zombies[zombie_num]);
		} else {
			destroy_ball = Grounded_Map_OutOfBounds(&fb->r) || Grounded_Map_FindMinCollition(&fb->r, &tmprect);
		}

		if (destroy_ball) {
			map_data.fireballs[i] = map_data.fireballs[map_data.fireballs_count - 1];
			map_data.fireballs_count--;
			break;
		}
	}

	/* Elevators. */

	for (i = 0; i < map_data.elevators_count; ++i) {
		Grounded_Elevator_Update(&map_data.elevators[i]);
	}
}

int Grounded_Map_OutOfBounds(SDL_Rect *r) {
	return r->x < 0 || r->y < 0 || r->x + r->w > W || r->y + r->h > H;
}

int Grounded_Map_FindMinCollition(SDL_Rect *r, SDL_Rect *minpos) {
	return Grounded_Map_FindMinCollition_ExceptElevator(r, minpos, NULL);
}

int Grounded_Map_FindMinCollition_ExceptElevator(SDL_Rect *r, SDL_Rect *minpos, const grounded_elevator_t* el) {
	int x, y, minx, miny, maxx, maxy, found=0;

	minx = r->x / IMG_W;
	miny = r->y / IMG_H;
	maxx = minx + 1;
	maxy = miny + 1;

	minpos->w = IMG_W;
	minpos->h = IMG_H;

	for (y = miny; y <= maxy && !found; ++y) {
		for (x = minx; x <= maxx && !found; ++x) {
			if (map_data.tiles[y][x] == TILE_FLOOR) {
				minpos->x = x * IMG_W;
				minpos->y = y * IMG_H;

				if (Util_Rect_Intersects(r, minpos)) {
					found = 1;
				}
			}
		}
	}

	/* Elevators. */

	if (!found) {
		minpos->x = W;
		minpos->y = H;
	}

	for (x = 0; x < map_data.elevators_count; ++x) {
		SDL_Rect *relev = &map_data.elevators[x].r;
		if ((&map_data.elevators[x] != el) && (Util_Rect_Intersects(r, relev))) {
			if (relev->y < minpos->y) {
				*minpos = *relev;
				found = 1;
			}
		}
	}

	return found;
}

void Grounded_Ent_Reset(grounded_ent_t *e, enum ENT_TYPE t) {
	Util_Init_Rect(&e->r, 0, 0, IMG_W, IMG_H);
	e->type = t;

	switch (t) {
	case ENT_TYPE_PLAYER:
		e->delta_move = DELTA_MOVE_PLAYER;
		break;

	case ENT_TYPE_ZOMBIE:
		e->delta_move = DELTA_MOVE_ZOMBIE + (rand() % 2);
		break;
	}
}

void Grounded_Ent_Move(grounded_ent_t *e, int left, int right) {
	SDL_Rect newpos = e->r, minpos;

	if (left) {
		newpos.x -= e->delta_move;
		e->heading = HEADING_LEFT;
	} else if (right) {
		newpos.x += e->delta_move;
		e->heading = HEADING_RIGHT;
	} else {
		return;
	}

	if (!Grounded_Map_OutOfBounds(&newpos)) {
		if (!Grounded_Map_FindMinCollition(&newpos, &minpos)) {
			e->r = newpos;
		}
	}
}

void Grounded_Ent_Die(grounded_ent_t *e) {
	switch (e->type) {
	case ENT_TYPE_PLAYER:
		Grounded_Ent_Reset(e, e->type);
		break;

	case ENT_TYPE_ZOMBIE:
		Grounded_Zombie_Remove(e);
		break;
	}
}

void Grounded_Zombie_Init(grounded_ent_t *e, int x, int y) {
	Grounded_Ent_Reset(e, ENT_TYPE_ZOMBIE);
	Util_Init_Rect(&e->r, x, y, IMG_W, IMG_H);
}

void Grounded_Zombie_Remove(grounded_ent_t *e) {
	int i = e - map_data.zombies;

	map_data.zombies[i] = map_data.zombies[--map_data.zombies_count];
}

void Grounded_Ent_Update(grounded_ent_t *e) {
	SDL_Rect newpos = e->r;
	SDL_Rect minpos;
	int ret;

	/* Furea de mapa. */

	if (Grounded_Map_OutOfBounds(&e->r)) {
		Grounded_Ent_Die(e);
		return;
	}

	/* Estoy sobre un ascensor? */

	ret = Grounded_Map_FindMinCollition(&e->r, &minpos);

	if (ret) {
		if (e->r.y < minpos.y) {
			/* Colision abajo. */
			e->r.y = minpos.y - minpos.h;
		} else {
			/* Colision arriba. */
			if (minpos.y + minpos.h - e->r.y > e->delta_move) {
				Grounded_Ent_Die(e);
				return;
			}
		}
	}

	/* Salto */

	if (e->jumping) {
		newpos.y -= e->delta_move;
		e->jump_pos -= e->delta_move;
		if (e->jump_pos <= 0) {
			e->jumping = 0;
		}
	} else {
		newpos.y += e->delta_move;
	}

	/* pies en la tierra? */

	e->grounded = Grounded_Map_FindMinCollition(&newpos, &minpos);

	if (!e->grounded) {
		/* Caida libre, actualizo la posicion. */
		e->r = newpos;
	} else {
		/* Colision con un solido. */
		if (e->r.y < minpos.y) {
			/* Bloqueo abajo. */
			e->r.y = minpos.y - e->r.h;
		} else {
			/* Bloqueo arriba. */
			if ((e->jumping) && (minpos.y + minpos.h - e->r.y <= e->delta_move)) {
				e->r.y = minpos.y + minpos.h;
			}
		}

		if (e->jumping) {
			e->jumping = 0;
			e->jump_pos = 0;
		}
	}

	/* Collect items. */

	if (e->type == ENT_TYPE_PLAYER) {
		Grounded_Ent_CollectItems(e);
	}
}

/**
 * Busca una colision entre el rect indicado por parametro y los zombies del mapa.
 * Devuelve el indice del primer zombie encontrado, o -1 en caso de no existir colision.
 *
 */
int Grounded_Map_FindZombieCollition(SDL_Rect *r) {
	int i;

	for (i = 0; i < map_data.zombies_count; ++i) {
		if (Util_Rect_Intersects(&map_data.zombies[i].r, r)) {
			return i;
		}
	}

	return -1;
}

void Grounded_Ent_CollectItems(grounded_ent_t* e) {
	int i;

	/* Zombies */

#if 1
	if (Grounded_Map_FindZombieCollition(&e->r) != -1) {
		Grounded_Ent_Die(e);
		return;
	}
#endif

	/* Apples */

	for (i = 0; i < map_data.apples_count; ++i) {
		if (Util_Rect_Intersects(&map_data.apples[i], &e->r)) {
			int x, y;

			x = map_data.apples[i].x / IMG_W;
			y = map_data.apples[i].y / IMG_H;

			if (map_data.tiles[y][x] == TILE_APPLE)
				map_data.tiles[y][x] = TILE_EMPTY;

			map_data.apples[i] = map_data.apples[map_data.apples_count - 1];
			map_data.apples_count--;
			e->points++;
			break;
		}
	}

	/* Exit */

	for (i = 0; i < map_data.exits_count; ++i) {
		if (Util_Rect_Intersects(&map_data.exits[i], &e->r)) {
			Grounded_Load_NextMap();
			return;
		}
	}

}

void Grounded_Ent_Jump(grounded_ent_t *e) {
	if (!e->jumping && e->grounded) {
		e->jumping = 1;
		e->jump_pos = (IMG_H * 3) / 2;
	}
}

void Grounded_Ent_FireBall(grounded_ent_t *e) {
	if (map_data.fireballs_count < MAP_MAX_FIREB) {
		grounded_object_t *fb = &map_data.fireballs[map_data.fireballs_count++];

		fb->dir = e->heading;
		fb->type = 0;
		fb->r = e->r;
	}
}

void Grounded_Update_Player(void) {
	Grounded_Ent_Update(&map_data.player);
	Grounded_Ent_Move(&map_data.player, teclas[SDLK_LEFT], teclas[SDLK_RIGHT]);
}

void Grounded_Game_Loop(void) {
	while (run) {
		Grounded_Handle_Events();

		Grounded_Map_Update();

		Grounded_Update_Player();

		Grounded_Draw_Map();

		SDL_Flip(screen);
		Grounded_Limit_FPS();
	}
}

void Grounded_Quit(void) {
	if (imgs) {
		SDL_FreeSurface(imgs);
		imgs = 0;
	}

	SDL_Quit();
}

int main(int argc, char** argv) {

#if 0
	Test_Rect_Intersects();
#endif

	Grounded_Init();
	Grounded_Load_Files();
	Grounded_Game_Loop();
	Grounded_Quit();

	return EXIT_SUCCESS;
}



