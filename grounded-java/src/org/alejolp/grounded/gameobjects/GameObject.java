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

import org.alejolp.grounded.GameCanvas;

public class GameObject {
	
	public int imgNum;
	public int x, y;
	public boolean remove = false;
	
	public GameObject(int imgNum, int x, int y) {
		this.imgNum = imgNum;
		this.x = x;
		this.y = y;
	}
	
	public void draw(GameCanvas canvas) {
		canvas.drawTileNum(imgNum, x, y);
	}
	
	public void update() {
		// Nothing to do here. This is meant to be @Overriden by subclasses.
	}
	
	public void onTimer() {
		// Nothing to do here. This is meant to be @Overriden by subclasses.
	}
}
