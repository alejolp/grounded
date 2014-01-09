#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Grounded. Pequeño juego de plataformas.

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
 *
"""

import pygame
from pygame.constants import *

import sys, os

W, H = 640, 480
TILE_SIZE = 32
DELTA_MOVE_JUGAD = 8
DELTA_MOVE_ZOMBI = 2
DELTA_MOVE_FIREB = 12
GAME_FPS = 25
GAME_TITLE = "Grounded"
GAME_INSTRUCT = "Mover: Flechas, Saltar: Espacio, Disparar: Z"

MAP = None

BLACK = 0, 0, 0
WHITE = 255, 255, 255
RED = 192, 64, 64
GREEN = 64, 192, 64
BLUE = 64, 64, 192

TECLAS_DIAMANTE = [K_LEFT, K_UP, K_RIGHT, K_DOWN]

TILES_IMG = None
TILES_IMG_RECTS = []

class GameMap(object):
    def __init__(self, n):
        self.n = n
        self.data = None

        self.load_map_file()

    def load_map_file(self):
        self.data = [list(x) for x in \
            open("maps/%d.txt" % self.n)]
        self._collect_items()

    def draw(self, screen):
        for y, line in enumerate(self.data):
            for x, tile in enumerate(line):
                p = (x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE)
                if tile == '.':
                    screen.blit(TILES_IMG, p, TILES_IMG_RECTS[1])
                else:
                    screen.blit(TILES_IMG, p, TILES_IMG_RECTS[0])

        todraw = [self.items, self.exits, self.elevs, self.zombi, \
            self.fireb, self.speci]
        for a in todraw:
            for i in a:
                i.draw(screen)

    def update(self):
        # Fin del mapa?
        """
        if len(self.items) == 0:
            self.load_map_file()
        """

        for a in [self.elevs, self.zombi, self.fireb]:
            for i in a:
                i.update()

    def _collect_items(self):
        self.items = []
        self.exits = []
        self.elevs = []
        self.zombi = []
        self.fireb = []
        self.speci = []

        for y, line in enumerate(self.data):
            for x, tile in enumerate(line):
                p = (x * TILE_SIZE, y * TILE_SIZE)
                if tile == '+':
                    self.items.append(Item(p))
                elif tile == '*':
                    self.exits.append(Exit(p))
                elif tile == '!' or tile == '?':
                    self.elevs.append(Elevator(p, (tile == '?')))
                elif tile == 'z':
                    self.zombi.append(Zombie(p))
                elif tile == '$':
                    self.speci.append(ZombiePortal(p))

    def find_collisions(self, r):
        rects = []
        x_start = max(r.left / TILE_SIZE - 1, 0)
        x_end = min(x_start + 3, W / TILE_SIZE)

        y_start = max(r.top / TILE_SIZE - 1, 0)
        y_end = min(y_start + 3, H / TILE_SIZE)

        for y2 in xrange(y_start, y_end):
            for x2 in xrange(x_start, x_end):
                 tile = self.data[y2][x2]
                 if tile == '.':
                    x, y = x2 * TILE_SIZE, y2 * TILE_SIZE
                    tile = pygame.rect.Rect(x, y, TILE_SIZE, TILE_SIZE)
                    if r.colliderect(tile):
                        rects.append(tile)

        for i in self.elevs:
            if i.rect.colliderect(r):
                rects.append(i.rect)

        return rects

    def find_collisions_with(self, pos, seq):
        for idx, i in enumerate(seq):
            if i.rect.colliderect(pos):
                return idx
        return None

    def find_items_collision(self, pos):
        idx = self.find_collisions_with(pos, self.items)
        if idx is not None:
            i = self.items[idx]
            del self.items[idx]
            self._on_item_collect(i)
            return i
        return None

    def find_exits_collision(self, pos):
        idx = self.find_collisions_with(pos, self.exits)
        if idx is not None:
            return self.exits[idx]
        return None

    def find_zombi_collision(self, pos):
        idx = self.find_collisions_with(pos, self.zombi)
        if idx is not None:
            return self.zombi[idx]
        return None

    def _on_item_collect(self, item):
        for x in self.speci:
            x._on_item_collect(item)

    def on_timer(self):
        for x in self.speci:
            x.on_timer()

    def out_of_bounds(self, pos):
        return pos.left < 0 or pos.right > W or pos.top < 0 or pos.bottom > H

    def next_map(self):
        self.n += 1
        self.load_map_file()

    def fire(self, pos, heading_left):
        self.fireb.append(FireBall(pos, heading_left))

class GameObject(object):
    def __init__(self, pos, imgnum):
        self.rect = pygame.rect.Rect(pos, (TILE_SIZE, TILE_SIZE))
        self.imgnum = imgnum

    def draw(self, screen):
        screen.blit(TILES_IMG, self.rect, TILES_IMG_RECTS[self.imgnum])

class Item(GameObject):
    def __init__(self, pos):
        super(Item, self).__init__(pos, 2)

class Exit(GameObject):
    def __init__(self, pos):
        super(Exit, self).__init__(pos, 4)

class Elevator(GameObject):
    def __init__(self, pos, bounded):
        super(Elevator, self).__init__(pos, 5)
        self.bajando = True
        self.bounded = bounded
        self.nivel_lim = 480 - TILE_SIZE * 3
        self.nivel = self.nivel_lim

    def update(self):
        if self.bounded:
            newpos = pygame.rect.Rect(self.rect)
            newpos.top += (1 if self.bajando else (-1))
            rects = MAP.find_collisions(newpos)
            if len(rects) > 1 or MAP.out_of_bounds(newpos):
                self.bajando = not self.bajando
            else:
                self.rect = newpos
        else:
            if self.bajando:
                self.rect.top += 1
            else:
                self.rect.top -= 1

            self.nivel -= 1
            if self.nivel == 0:
                self.nivel = self.nivel_lim
                self.bajando = not self.bajando

class FireBall(GameObject):
    def __init__(self, pos, heading_left):
        super(FireBall, self).__init__(pos, 7)
        self.heading_left = heading_left

    def update(self):
        newpos = pygame.rect.Rect(self.rect)
        newpos[0] += DELTA_MOVE_FIREB * (-1 if self.heading_left else 1)

        rects = MAP.find_collisions(newpos)
        if MAP.out_of_bounds(newpos) or len(rects):
            MAP.fireb.remove(self)
        else:
            self.rect = newpos

        z = MAP.find_zombi_collision(self.rect)
        if z is not None:
            try:
                MAP.fireb.remove(self)
            except ValueError:
                pass
            MAP.zombi.remove(z)

class GameEnt(GameObject):
    def __init__(self, pos, imgnum, delta_move):
        super(GameEnt, self).__init__(pos, imgnum)
        self.jumping = False
        self.jump_pos = 0
        self.grounded = False
        self.heading_left = True
        self.delta_move = delta_move

    def jump(self):
        if not self.jumping and self.grounded:
            self.jumping = True
            self.jump_pos = TILE_SIZE + TILE_SIZE / 2

    def fire(self):
        MAP.fire(self.rect.topleft, self.heading_left)

    def reset(self):
        pass

    def _items_collision(self):
        pass

    def update(self):
        newpos = pygame.rect.Rect(self.rect)

        # Fuera de mapa
        if MAP.out_of_bounds(self.rect):
            self.reset()
            return

        # Cambio en el terreno
        rects = MAP.find_collisions(self.rect)
        if len(rects) != 0:
            t = min(rects, key=lambda x: x.top)
            if self.rect.top < t.top:
                self.rect.bottom = t.top
            elif t.bottom - self.rect.top > self.delta_move:
                self.reset()
                return

        # Salta
        if self.jumping:
            newpos.top -= self.delta_move
            self.jump_pos -= self.delta_move
            if self.jump_pos == 0:
                self.jumping = False
        else:
            # Cae
            newpos.top += self.delta_move

        rects = MAP.find_collisions(newpos)
        self.grounded = (len(rects) != 0)

        if not self.grounded:
            self.rect = newpos
        else:
            # Bloque abajo
            t = min(rects, key=lambda x: x.top)
            if self.rect.top < t.top:
                self.rect.bottom = t.top
            else:
                # Bloque arriba
                if self.jumping:
                    if t.bottom - self.rect.top <= self.delta_move:
                        self.rect.top = t.bottom

            if self.jumping:
                self.jumping = False
                self.jump_pos = 0

        self._items_collision()

    def move(self, teclas):
        newpos = pygame.rect.Rect(self.rect)

        if teclas[K_LEFT]:
            newpos.left -= self.delta_move
            self.heading_left = True
        elif teclas[K_RIGHT]:
            newpos.left += self.delta_move
            self.heading_left = False

        if not MAP.out_of_bounds(newpos):
            rects = MAP.find_collisions(newpos)
            if len(rects) == 0:
                self.rect = newpos

class Zombie(GameEnt):
    def __init__(self, pos):
        super(Zombie, self).__init__(pos, 6, DELTA_MOVE_ZOMBI)
        self.derecha = True

    def reset(self):
        MAP.zombi.remove(self)

    def update(self):
        oldpos = self.rect.center
        super(Zombie, self).update()

        k = {K_LEFT: False, K_RIGHT: False}
        if self.derecha:
            k[K_LEFT] = True
        else:
            k[K_RIGHT] = True

        super(Zombie, self).move(k)

        if oldpos == self.rect.center:
            self.derecha = not self.derecha

class Jugador(GameEnt):
    def __init__(self):
        super(Jugador, self).__init__((0, 0), 3, DELTA_MOVE_JUGAD)

    def reset(self):
        self.rect.topleft = (0, 0)
        self.jumping = False
        self.grounded = False

    def _items_collision(self):
        if MAP.find_zombi_collision(self.rect) is not None:
            self.reset()
            return

        i = MAP.find_items_collision(self.rect)
        e = MAP.find_exits_collision(self.rect)

        if e is not None:
            MAP.next_map()
            self.reset()

class ZombiePortal(GameObject):
    def __init__(self, pos):
        super(ZombiePortal, self).__init__(pos, 2)

    def _on_item_collect(self, item):
        self.on_timer()

    def on_timer(self):
        if len(MAP.items) == 0 and len(MAP.zombi) < 40:
            newpos = pygame.rect.Rect(self.rect)
            MAP.zombi.append(Zombie(newpos.topleft))

def main():
    global TILES_IMG, TILES_IMG_RECTS, MAP

    pygame.init()

    pygame.display.set_caption(GAME_TITLE)
    screen = pygame.display.set_mode((W, H), 0, 0)
    
    TILES_IMG = pygame.image.load("img/tiles.png").convert()
    TILES_IMG.set_colorkey(TILES_IMG.map_rgb((255, 0, 255)))
    TILES_IMG_RECTS = [pygame.rect.Rect(x * TILE_SIZE, 0, \
        TILE_SIZE, TILE_SIZE) for x in xrange(8)]

    MAP = GameMap(1)

    clock = pygame.time.Clock()
    salir = False
    jugador = Jugador()

    teclas = dict(zip(TECLAS_DIAMANTE, [False] * 4))

    f = pygame.font.SysFont("", 24)

    ftext, ftextpos = [], []

    ftext.append(f.render(GAME_TITLE, True, BLACK, WHITE))
    ftextpos.append(ftext[-1].get_rect(center=(W / 2, H / 2)))
    ftext.append(f.render(GAME_INSTRUCT, True, BLACK, WHITE))
    ftextpos.append(ftext[-1].get_rect(center=(W / 2, H / 2 + ftextpos[-1].height)))

    pygame.time.set_timer(USEREVENT, 1000)
    start_t = pygame.time.get_ticks()

    while not salir:
        # Events

        for e in pygame.event.get():
            if e.type == QUIT:
                salir = True
            elif e.type in [KEYDOWN, KEYUP] and e.key in TECLAS_DIAMANTE:
                teclas[e.key] = (e.type == KEYDOWN)
            elif e.type == KEYDOWN:
                if e.key == K_SPACE:
                    jugador.jump()
                elif e.key == K_z:
                    jugador.fire()
                elif e.key == K_ESCAPE:
                    salir = True
                elif e.key == K_F5:
                    pygame.display.toggle_fullscreen()
                elif e.key == K_F1:
                    MAP.next_map()
                    jugador.reset()
                else:
                    print "Tecla:", e.unicode
            elif e.type == (USEREVENT + 0):
                MAP.on_timer()

        MAP.update()
        jugador.update()

        # Move

        jugador.move(teclas)

        # Draw

        MAP.draw(screen)
        jugador.draw(screen)

        if pygame.time.get_ticks() - start_t < 10000:
            for tt, ttp in zip(ftext, ftextpos):
                screen.blit(tt, ttp)

        pygame.display.flip()
        clock.tick(GAME_FPS)

    pygame.quit()

if __name__ == '__main__':
    main()

