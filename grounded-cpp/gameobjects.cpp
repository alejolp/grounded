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

#include "game.hpp"
#include "gameobjects.hpp"
#include "constants.hpp"


namespace grounded {

gameobject::gameobject(std::size_t imgnum, int x, int y)
    : destroy_(false), imgnum_(imgnum), x_(x), y_(y) {}

gameobject::~gameobject() {}

void gameobject::on_timer() {}

void gameobject::update() {}

ent::ent(std::size_t imgnum, int x, int y, int speed)
    : gameobject(imgnum, x, y), speed_(speed), jumping_(false), 
      jump_pos_(0), grounded_(false), left_(true) {}

void ent::update() {
    game* g = game::instance();
    gamemap* m = g->map();
    int newx = x_, newy = y_;
    int miny;
    
    if (m->out_of_bounds(x_, y_)) {
        reset();
        return;
    }
    
    miny = m->find_min_collision_y(x_, y_);
    if (miny != WINDOW_HEIGHT) {
        if (y_ < miny) {
            y_ = miny - TILE_SIZE;
        } else {
            reset();
            return;
        }
    }
    
    if (jumping_) {
        newy -= speed_;
        jump_pos_ -= speed_;
        if (jump_pos_ <= 0) {
            jumping_ = false;
            jump_pos_ = 0;
        }
    } else {
        newy += speed_;
    }
    
    miny = m->find_min_collision_y(newx, newy);
    grounded_ = (miny != WINDOW_HEIGHT);
    
    if (!grounded_) {
        x_ = newx;
        y_ = newy;
    } else {
        if (y_ < miny) {
            y_ = miny - TILE_SIZE;
        } else if (jumping_) {
            if ((miny + TILE_SIZE - y_) <= speed_) {
                y_ = miny + TILE_SIZE;
            }
        }
        
        if (jumping_) {
            jumping_ = false;
            jump_pos_ = 0;
        }
    }
    
    check_item_collision();
}

void ent::move(bool move_left) {
    game* g = game::instance();
    gamemap* m = g->map();
    int newx = x_, newy = y_;

    if (move_left)
        newx -= speed_;
    else
        newx += speed_;
    left_ = move_left;
    
    if (!m->out_of_bounds(newx, newy)) {
        if (!m->has_collision(newx, newy)) {
            x_ = newx;
            y_ = newy;
        }
    }
}

void ent::reset() {
    // Nothing.
}

void ent::fire() {
    gamemap* m = game::instance()->map();
    m->fireballs().push_back(gameobject_ptr(new fireball(x_, y_, left_)));
}

void ent::jump() {
    if (!jumping_ && grounded_) {
        jumping_ = true;
        jump_pos_ = TILE_SIZE + TILE_SIZE / 2;
    }
}

void ent::check_item_collision() {
    // Nothing.
}

void elevator::update() {
    gamemap* m = game::instance()->map();
    
    if (bounded_) {
        int newy = y_ + ((down_) ? 1 : (-1));
        
        if (m->has_collision(x_, newy, this) || m->out_of_bounds(x_, newy)) {
            down_ = !down_;
        } else {
            y_ = newy;
        }
    } else {
        y_ += ((down_) ? 1 : (-1));
        level_--;
        if (level_ <= 0) {
            level_ = level_lim_;
            down_ = !down_;
        }
    }
}

void fireball::update() {
    game* g = game::instance();
    gamemap* m = g->map();
    int newx = x_, newy = y_;
    
    newx += left_ ? -SPEED_FIREB : SPEED_FIREB;
    
    if (m->out_of_bounds(newx, newy) || m->has_collision(newx, newy)) {
        mark_for_destroy();
    } else {
        x_ = newx;
        y_ = newy;
    }
    
    gameobject* z = m->find_zombie_collision(newx, newy);
    if (z) {
        z->mark_for_destroy();
        mark_for_destroy();
    }
}

void zombie::update() {
    int oldx = x_, oldy = y_;
    ent::update();
    ent::move(left_);
    if (x_ == oldx && y_ == oldy)
        left_ = !left_;
}

void zombie::reset() {
    mark_for_destroy();
}

void player::check_item_collision() {
    gamemap* m = game::instance()->map();

    if (m->has_zombie_collision(x_, y_)) {
        reset();
        return;
    }
    
    m->collect_items(x_, y_);
    
    if (m->has_exit_collision(x_, y_)) {
        m->next_map();
        reset();
    }
}

void player::reset() {
    x_ = 0;
    y_ = 0;
    jumping_ = false;
    grounded_ = false;
}


void zombieportal::on_timer() {
    game* g = game::instance();
    gamemap* m = g->map();
    
    if (m->items().size() == 0 && m->zombies().size() < 40) {
        m->zombies().push_back(gameobject_ptr(new zombie(x_, y_)));
    }
}

}
