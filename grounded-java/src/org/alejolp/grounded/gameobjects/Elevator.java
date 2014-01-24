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

public class Elevator extends GameObject {

	private boolean bounded;
	private boolean down = true;
	private int levellim = Constants.PANEL_HEIGHT - Constants.TILE_SIZE * 3;
	private int level = levellim;
	
	public Elevator(int x, int y, boolean bounded) {
		super(Constants.ID_ELEVATOR, x, y);
		this.bounded = bounded;
	}
	
	@Override
	public void update() {
		if (bounded) {
			int newy = y;
			newy += down ? 1 : -1;
			if (Game.getInstance().map.hasCollisionExcept(x, newy, this) || Game.getInstance().map.outOfBounds(x, newy)) {
				down = !down;
			} else {
				y = newy;
			}
		} else {
			y += down ? 1 : -1;
			level--;
			if (level == 0) {
				level = levellim;
				down = !down;
			}
		}
	}

}
