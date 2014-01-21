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

public class ZombiePortal extends GameObject {

	public ZombiePortal(int x, int y) {
		super(Constants.ID_ITEM, x, y);
	}

	public void onItemCollect() {
		onTimer();
	}
	
	@Override
	public void onTimer() {
		Game g = Game.getInstance();
		if (g.map.items.size() == 0 && g.map.zombies.size() < 40) {
			Zombie zz = new Zombie(x, y);
			
			zz.left = g.r.nextBoolean();
			g.map.zombies.add(zz);
		}
	}
}
