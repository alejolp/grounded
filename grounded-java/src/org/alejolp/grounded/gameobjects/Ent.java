package org.alejolp.grounded.gameobjects;

/*
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

import org.alejolp.grounded.Constants;
import org.alejolp.grounded.Game;

public class Ent extends GameObject {

	public int speed = 0; // delta_move in .py
	public boolean jumping = false;
	public int jump_pos = 0;
	public boolean grounded = false;
	public boolean left = true;
	
	public Ent(int imgNum, int x, int y, int speed) {
		super(imgNum, x, y);
		this.speed = speed;
	}

	@Override
	public void update() {
		Game g = Game.getInstance();
		int newx = x, newy = y;
		int miny;
		
		// Out of the map
		if (g.map.outOfBounds(x, y)) {
			reset();
			return;
		}
		
		// Update position on elevator movement
		miny = g.map.findMinCollisionY(x, y);
		if (miny != Constants.PANEL_HEIGHT) {
			if (y < miny) {
				y = miny - Constants.TILE_SIZE;
			} else if ((miny + Constants.TILE_SIZE) - y > speed) {
				reset();
				return;
			}
		}
		
		// Jump.
		if (jumping) {
			newy -= speed;
			jump_pos -= speed;
			if (jump_pos <= 0) {
				jumping = false;
				jump_pos = 0;
			}
		} else {
			newy += speed;
		}
		
		miny = g.map.findMinCollisionY(newx, newy);
		grounded = (miny != Constants.PANEL_HEIGHT);
		
		if (!grounded) {
			x = newx;
			y = newy;
		} else {
			if (y < miny) {
				y = miny - Constants.TILE_SIZE;
			} else if (jumping) {
				if ((miny + Constants.TILE_SIZE) - y <= speed) {
					y = miny + Constants.TILE_SIZE;
				}
			}
			
			if (jumping) {
				jumping = false;
				jump_pos = 0;
			}
		}
		
		checkItemCollision();
	}
	
	public void move(boolean moveLeft) {
		Game g = Game.getInstance();
		int newx = x, newy = y;
		
		if (moveLeft) {
			newx -= speed;
		} else {
			newx += speed;
		}
		left = moveLeft;
		
		if (!g.map.outOfBounds(newx, newy)) {
			if (!g.map.hasCollision(newx, newy)) {
				this.x = newx;
				this.y = newy;
			}
		}
	}
	
	public void reset() {
		
	}
	
	public void jump() {
		if (!this.jumping && this.grounded) {
			this.jumping = true;
			this.jump_pos = Constants.TILE_SIZE + Constants.TILE_SIZE / 2;
		}
	}
	
	public void fire() {
		Game.getInstance().map.fire(this.x, this.y, this.left);
	}
	
	protected void checkItemCollision() {
		
	}
}
