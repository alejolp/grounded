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

public class FireBall extends GameObject {

	boolean left;
	
	public FireBall(int x, int y, boolean left) {
		super(Constants.ID_FIREBALL, x, y);
		this.left = left;
	}

	@Override
	public void update() {
		Game g = Game.getInstance();
		
		int newx = x, newy = y;
		
		newx += left ? -Constants.SPEED_FIREB : Constants.SPEED_FIREB;
		
		if (g.map.outOfBounds(newx, newy) || g.map.hasCollision(newx, newy)) {
			// g.map.fireballs.remove(this);
			remove = true;
		} else {
			x = newx;
			y = newy;
		}
		
		GameObject z = g.map.findZombieCollision(newx, newy);
		if (z != null) {
			// g.map.fireballs.remove(this);
			remove = true;
			// g.map.zombies.remove(z);
			z.remove = true;
		}
	}
}
