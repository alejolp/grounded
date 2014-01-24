package org.alejolp.grounded;

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

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import org.alejolp.grounded.gameobjects.Elevator;
import org.alejolp.grounded.gameobjects.Exit;
import org.alejolp.grounded.gameobjects.FireBall;
import org.alejolp.grounded.gameobjects.GameObject;
import org.alejolp.grounded.gameobjects.Item;
import org.alejolp.grounded.gameobjects.Zombie;
import org.alejolp.grounded.gameobjects.ZombiePortal;

public class Map {
	
	public char[][] data;
	public int W, H;
	public int currentMapNum = 0;
	
	public LinkedList<GameObject> items = new LinkedList<GameObject>();
	public ArrayList<GameObject> zombies = new ArrayList<GameObject>();
	public ArrayList<GameObject> exits = new ArrayList<GameObject>();
	public ArrayList<GameObject> elevators = new ArrayList<GameObject>();
	public ArrayList<GameObject> specials = new ArrayList<GameObject>();
	public ArrayList<GameObject> fireballs = new ArrayList<GameObject>();
	
	public Map() {
		loadMap(1);
		
		assert !tileIntersection(0, 0, 32, 0);
		assert !tileIntersection(0, 0, 32, 32);
		assert !tileIntersection(0, 0, 0, 32);
		
		assert !tileIntersection(32, 0, 0, 0);
		assert !tileIntersection(32, 32, 0, 0);
		assert !tileIntersection(0, 32, 0, 0);
	}
	
	public void nextMap() {
		loadMap(currentMapNum + 1);
	}
	
	private void loadMap(int n) {
		int x, y;
		
		if (n == Data.MAP_DATA.length)
			return;
		
		currentMapNum = n;
		String[] d = Data.MAP_DATA[n].split("\n");
		data = new char[d.length][];
		
		for (y = 0; y < d.length; ++y) {
			data[y] = d[y].toCharArray();
		}
		
		W = data[0].length;
		H = data.length;
		
		items.clear();
		zombies.clear();
		exits.clear();
		elevators.clear();
		specials.clear();
		fireballs.clear();
		
		for (y = 0; y < H; ++y) {
			for (x = 0; x < W; ++x) {
				int pixelx = x * Constants.TILE_SIZE, pixely = y * Constants.TILE_SIZE;
				
				switch (data[y][x]) {
				case '+': // item
					items.add(new Item(pixelx, pixely));
					break;
					
				case '*': // Exit
					exits.add(new Exit(pixelx, pixely));
					break;
					
				case '!': // Elevator 1, unbounded.
				case '?': // Elevator 2, bounded.
					elevators.add(new Elevator(pixelx, pixely, (data[y][x] == '?')));
					break;
					
				case 'z': // Zombie
					zombies.add(new Zombie(pixelx, pixely));
					break;
					
				case '$': // Zombie Portal
					specials.add(new ZombiePortal(pixelx, pixely));
					break;
				
				default:
					assert false;
				}
			}
		}
	}
	
	public void update() {
		update(elevators);
		update(zombies);
		update(fireballs);
	}
	
	public void onTimer() {
		for (Iterator<GameObject> it = specials.iterator(); it.hasNext(); ) {
			GameObject w = it.next();
			
			if (w.remove) {
				it.remove();
			} else {
				w.onTimer();
			}
		}	
	}
	
	private static final void update(List<GameObject> L) {
		for (Iterator<GameObject> it = L.iterator(); it.hasNext(); ) {
			GameObject w = it.next();
			
			if (w.remove) {
				it.remove();
			} else {
				w.update();
			}
		}		
	}
	
	private static final boolean tileIntersection(int x1, int y1, int x2, int y2) {
		final int t = Constants.TILE_SIZE;
		return (x1 + t > x2 && x1 < x2 + t && y1 + t > y2 && y1 < y2 + t);
	}
	
	public GameObject findZombieCollision(int x, int y) {
		for (GameObject w : zombies) {
			if (tileIntersection(x, y, w.x, w.y))
				return w;
		}
		return null;
	}

	public boolean hasZombieCollision(int x, int y) {
		return findZombieCollision(x, y) != null;
	}
	
	public boolean hasExitCollision(int x, int y) {
		for (GameObject w : exits) {
			if (tileIntersection(x, y, w.x, w.y))
				return true;
		}
		return false;
	}
	
	public boolean collectItems(int x, int y) {
		boolean ret = false;
		
		for (Iterator<GameObject> it = items.iterator(); it.hasNext(); ) {
			GameObject w = it.next();
			if (tileIntersection(x, y, w.x, w.y)) {
				it.remove();
				ret = true;
			}
		}
		
		return ret;

	}
	
	/**
	 * 
	 * @param ex
	 * @param ey
	 * @return Returns Constants.PANEL_HEIGHT on no collision.
	 */
	public int findMinCollisionY(int ex, int ey, Elevator exceptElevator) {
		int retminy = Constants.PANEL_HEIGHT;
		
		// A 32x32 rect can intersect with, at most, four 32x32 rects.
		int sx1 = ex / Constants.TILE_SIZE;
		int sy1 = ey / Constants.TILE_SIZE;
		int sx2 = ((ex % Constants.TILE_SIZE) != 0) ? sx1 + 2 : sx1 + 1;
		int sy2 = ((ey % Constants.TILE_SIZE) != 0) ? sy1 + 2 : sy1 + 1;
		int x, y;
		
		sx1 = Math.max(sx1, 0);
		sy1 = Math.max(sy1, 0);
		sx2 = Math.min(sx2, W);
		sy2 = Math.min(sy2, H);
		
		for (y = sy1; y < sy2; ++y) {
			for (x = sx1; x < sx2; ++x) {
				if (data[y][x] == '.' && tileIntersection(
						x * Constants.TILE_SIZE, 
						y * Constants.TILE_SIZE, ex, ey)) {
					retminy = Math.min(retminy, y * Constants.TILE_SIZE);
				}
			}
		}
		
		for (GameObject w : elevators) {
			if (w != exceptElevator && tileIntersection(ex, ey, w.x, w.y))
				retminy = Math.min(retminy, w.y);
		}
		
		return retminy;
	}
	
	public int findMinCollisionY(int ex, int ey) {
		return findMinCollisionY(ex, ey, null);
	}
	
	public boolean hasCollision(int x, int y) {
		return findMinCollisionY(x, y, null) != Constants.PANEL_HEIGHT;
	}
	
	public boolean hasCollisionExcept(int x, int y, Elevator exceptElevator) {
		return findMinCollisionY(x, y, exceptElevator) != Constants.PANEL_HEIGHT;
	}
	
	public boolean outOfBounds(int x, int y) {
		return (x < 0) || (y < 0) || 
				((x + Constants.TILE_SIZE) > Constants.PANEL_WIDTH) || 
				((y + Constants.TILE_SIZE) > Constants.PANEL_HEIGHT); 
	}
	
	public void fire(int x, int y, boolean left) {
		fireballs.add(new FireBall(x, y, left));
	}
}
