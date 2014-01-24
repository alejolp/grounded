/*
 * Grounded. Pequeño juego de plataformas.
 *
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
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <SDL.h>

#include "game.hpp"
#include "gameobjects.hpp"
#include "constants.hpp"
#include "renderer.hpp"


namespace grounded {

game game::s_game;


gamemap::gamemap() : mapnum_(0), w_(-1), h_(-1) {
    load_map(1);
    player_.reset(new player(0, 0));
}

gamemap::~gamemap() {
}

void gamemap::load_map(int n) {
    int x, y, pixelx, pixely;
    
    mapnum_ = n;
    
    items_.clear();
    exits_.clear();
    elevators_.clear();
    zombies_.clear();
    specials_.clear();
    m_.clear();
    
    std::stringstream s;
    s << "../common/maps/" << n << ".txt";
    std::ifstream f;
    f.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    f.open(s.str().c_str());
    
    h_ = 0;
    
    for (y=0, pixely=0; y<(WINDOW_HEIGHT/TILE_SIZE); ++y, ++h_, pixely += TILE_SIZE) {
        char line[256];
        std::size_t line_len;
        
        f.getline(line, SDL_arraysize(line) - 1, '\n');
        line_len = std::strlen(line);
        
        m_.resize(m_.size() + 1);
        VC& row = m_[m_.size() - 1];
        if (w_ == -1) {
            w_ = line_len;
        } else if (w_ != (int)line_len) {
            throw std::exception();
        }
        row.insert(row.begin(), line, line + line_len);
        
        for (x=0, pixelx=0; x<w_; ++x, pixelx += TILE_SIZE) {
            switch (row[x]) {
            case '+': // Item
                items_.push_back(gameobject_ptr(
                    new item(pixelx, pixely)));
                break;
                
            case '*': // Exit
                exits_.push_back(gameobject_ptr(
                    new exit(pixelx, pixely)));
                break;
                
            case '!': // Elevator
            case '?':
                elevators_.push_back(gameobject_ptr(
                    new elevator(pixelx, pixely, (row[x] == '?'))));
                break;
                
            case 'z': // Zombie
                zombies_.push_back(gameobject_ptr(
                    new zombie(pixelx, pixely)));
                break;
                
            case '$': // Zombie Portal
                specials_.push_back(gameobject_ptr(
                    new zombieportal(pixelx, pixely)));
                break;
            }
        }
    }
}

void gamemap::next_map() {
    load_map(mapnum_ + 1);
}

void gamemap::update() {
    update(elevators());
    update(zombies());
    update(fireballs());
    getplayer()->update();
}

void gamemap::update(VG& v) {
    VG::iterator it = v.begin();
    
    while (it != v.end()) {
        gameobject_ptr p = *it;
        if (p->is_destroyed()) {
            it = v.erase(it);
        } else {
            p->update();
            ++it;
        }
    }
}

void gamemap::on_timer() {
    VG::iterator it;
    for (it=specials().begin(); it != specials().end(); ++it) {
        gameobject_ptr p = *it;
        p->on_timer();
    }
}

int gamemap::find_collision(int x, int y, VG& v, gameobject* except) {
    VG::iterator it;
    int i = 0;
    SDL_Rect a, b;
    
    a.x = x;
    a.y = y;
    a.w = a.h = TILE_SIZE;
    b.w = b.h = TILE_SIZE;
    
    for (it=v.begin(); it != v.end(); ++it, ++i) {
        gameobject_ptr p = *it;
        b.x = p->x();
        b.y = p->y();
        if (p.get() != except && SDL_HasIntersection(&a, &b)) {
            return i;
        }
    }
    return -1;
}

bool gamemap::has_collision(int x, int y, gameobject* except) {
    return find_min_collision_y(x, y, except) != WINDOW_HEIGHT;
}

int gamemap::find_min_collision_y(int ex, int ey, gameobject* except) {
    int retminy = WINDOW_HEIGHT;

    // A 32x32 rect can intersect with, at most, four 32x32 rects.
    int sx1 = ex / TILE_SIZE;
    int sy1 = ey / TILE_SIZE;
    int sx2 = ((ex % TILE_SIZE) != 0) ? sx1 + 2 : sx1 + 1;
    int sy2 = ((ey % TILE_SIZE) != 0) ? sy1 + 2 : sy1 + 1;
    int x, y;
    SDL_Rect a, b;
    
    sx1 = std::max(sx1, 0);
    sy1 = std::max(sy1, 0);
    sx2 = std::min(sx2, width());
    sy2 = std::min(sy2, height());

    a.x = ex;
    a.y = ey;
    a.w = a.h = TILE_SIZE;
    b.w = b.h = TILE_SIZE;
    
    for (y = sy1; y < sy2; ++y) {
        for (x = sx1; x < sx2; ++x) {
            b.x = x * TILE_SIZE;
            b.y = y * TILE_SIZE;
            if (get(x, y) == '.' && SDL_HasIntersection(&a, &b)) {
                retminy = std::min(retminy, b.y);
            }
        }
    }

    VG::iterator it;
    for (it=elevators().begin(); it != elevators().end(); ++it) {
        gameobject_ptr p = *it;
        b.x = p->x();
        b.y = p->y();
        if (p.get() != except && SDL_HasIntersection(&a, &b)) {
            retminy = std::min(retminy, b.y);
        }
    }

    return retminy;
}

bool gamemap::has_zombie_collision(int x, int y) {
    return find_collision(x, y, zombies(), 0) != -1;
}

bool gamemap::has_exit_collision(int x, int y) {
    return find_collision(x, y, exits(), 0) != -1;
}

gameobject* gamemap::find_zombie_collision(int x, int y) {
    int idx = find_collision(x, y, zombies(), 0);
    return idx != -1 ? zombies_[idx].get() : 0;
}

bool gamemap::collect_items(int x, int y) {
    bool ret = false;
    VG::iterator it;
    SDL_Rect a, b;
    
    a.x = x;
    a.y = y;
    a.w = a.h = TILE_SIZE;
    b.w = b.h = TILE_SIZE;
    
    it=items().begin();
    while (it != items().end()) {
        gameobject_ptr p = *it;
        b.x = p->x();
        b.y = p->y();
        if (SDL_HasIntersection(&a, &b)) {
            // p->mark_for_destroy();
            it = items().erase(it);
            ret = true;
        } else {
            ++it;
        }
    }
    
    return ret;
}



game::game() : run_(true) {
    
}

void game::poll_events() {
    SDL_Event ev;
    
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            run_ = false;
            break;

        case SDL_KEYDOWN:
            {
                teclas_[ev.key.keysym.sym] = 1;
                switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE:
                    run_ = false;
                    break;
                    
                case SDLK_F1:
                    map()->next_map();
                    break;
                    
                case SDLK_F5:
                    /* renderer::instance()->fullscreen(); */
                    break;
                    
                case SDLK_SPACE:
                    map()->getplayer()->jump();
                    break;
                    
                case SDLK_z:
                    map()->getplayer()->fire();
                    break;
                }
            }
            break;
        
        case SDL_KEYUP:
            teclas_[ev.key.keysym.sym] = 0;
            break;
            
        case SDL_USEREVENT + 0:
            on_timer();
            break;
        }
    }
    
    if (teclas_[SDLK_LEFT] || teclas_[SDLK_RIGHT]) {
        map()->getplayer()->move(teclas_[SDLK_LEFT]);
    }
}

void game::update() {
    map_.update();
}

void game::on_timer() {
    map_.on_timer();
}

namespace {
    Uint32 timer_callback(Uint32 interval, void *param) {
        SDL_Event ev;

        memset(&ev, 0, sizeof(ev));
        ev.type = SDL_USEREVENT + 0;

        SDL_PushEvent(&ev);

        return interval;
    }
}

void game::init() {
    SDL_AddTimer(1000, timer_callback, 0);
}

}
