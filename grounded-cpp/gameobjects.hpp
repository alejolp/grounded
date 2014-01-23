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

#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <cstdlib>
#include <boost/noncopyable.hpp>

#include "constants.hpp"


namespace grounded {

class gameobject : private boost::noncopyable {
public:
    gameobject(std::size_t imgnum, int x, int y);
    virtual ~gameobject();
    
    int imgnum() const { return imgnum_; }
    int x() const { return x_; }
    int y() const { return y_; }
    
    virtual void on_timer();
    virtual void update();
    
protected:
    std::size_t imgnum_;
    int x_, y_;
};

class ent : public gameobject {
public:
    ent(std::size_t imgnum, int x, int y, int speed);

    virtual void update();
    virtual void move();
    virtual void reset();
    virtual void fire();
    virtual void jump();
    virtual void check_item_collision();
    
protected:
	int speed_; // delta_move in .py
	bool jumping_;
	int jump_pos_;
	bool grounded_;
	bool left_;
};

class item : public gameobject {
public:
    item(int x, int y) : gameobject(ID_ITEM, x, y) {}
};

class exit : public gameobject {
public:
    exit(int x, int y) : gameobject(ID_EXIT, x, y) {}
};

class elevator : public gameobject {
public:
    elevator(int x, int y, bool bounded) : gameobject(ID_ELEVATOR, x, y), bounded_(bounded) {}
    
    virtual void update();
    
private:
    bool bounded_;
};

class fireball : public gameobject {
public:
    fireball(int x, int y, bool left) : gameobject(ID_FIREBALL, x, y), left_(left) {}
    
    virtual void update();
    
private:
    bool left_;
};

class zombie : public ent {
public:
    zombie(int x, int y) : ent(ID_ZOMBIE, x, y, SPEED_ZOMBIE) {}
    
    virtual void update();
    virtual void reset();
};

class player : public ent {
public:
    player(int x, int y) : ent(ID_PLAYER, x, y, SPEED_PLAYER) {}
    
    virtual void check_item_collision();
    virtual void reset();
};

class zombieportal : public gameobject {
public:
    zombieportal(int x, int y) : gameobject(ID_ITEM, x, y) {}
    
    virtual void on_timer();
};

}

#endif
