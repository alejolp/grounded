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

public class Zombie extends Ent {

	public Zombie(int x, int y) {
		super(Constants.ID_ZOMBIE, x, y, Constants.SPEED_ZOMBIE);
	}

	@Override
	public void update() {
		int oldx = x, oldy = y;
		
		super.update();
		super.move(left);
		
		if (x == oldx && y == oldy) {
			// Not moving.
			left = !left;
		}
	}
	
	@Override
	public void reset() {
		// Game.getInstance().map.zombies.remove(this);
		// Prevent a ConcurrentModificationException error
		this.remove = true;
	}
}
