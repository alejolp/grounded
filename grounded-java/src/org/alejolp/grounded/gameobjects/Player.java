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

public class Player extends Ent {

	public Player(int x, int y) {
		super(Constants.ID_PLAYER, x, y, Constants.SPEED_PLAYER);
	}

	@Override
	public void reset() {
		this.x = 0;
		this.y = 0;
		this.jumping = false;
		this.grounded = false;
	}
	
	@Override
	protected void checkItemCollision() {
		Game g = Game.getInstance();
		
		if (g.map.hasZombieCollision(x, y)) {
			reset();
			return;
		}
		
		g.map.collectItems(x, y);
		
		if (g.map.hasExitCollision(x, y)) {
			g.map.nextMap();
			reset();
		}
	}
}
