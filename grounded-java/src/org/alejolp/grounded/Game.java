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

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.HashMap;
import java.util.Random;

import org.alejolp.grounded.gameobjects.Player;

public class Game implements KeyListener {

	private static Game instance = new Game();
	
	public Random r = new Random();
	public Map map;
	public Player p = new Player(0, 0);
	
	private HashMap<Integer, Boolean> keys = new HashMap<Integer, Boolean>();
	private long lastTimerTime = 0;
	
	public static Game getInstance() {
		return instance;
	}
	
	private Game() {
		map = new Map();
		
		keys.put(KeyEvent.VK_LEFT, Boolean.FALSE);
		keys.put(KeyEvent.VK_RIGHT, Boolean.FALSE);
		keys.put(KeyEvent.VK_Z, Boolean.FALSE);
		keys.put(KeyEvent.VK_SPACE, Boolean.FALSE);
		keys.put(KeyEvent.VK_F5, Boolean.FALSE);
	}
	
	public void update() {
		long ct = System.currentTimeMillis();
		
		map.update();
		p.update();
		
		if (ct - lastTimerTime >= 1000) {
			lastTimerTime = ct;
			onTimer();
		}
		
		if (keys.get(KeyEvent.VK_LEFT)) {
			p.move(true);
		} else if (keys.get(KeyEvent.VK_RIGHT)) {
			p.move(false);
		}
		
		if (keys.get(KeyEvent.VK_Z)) {
			p.fire();
		} else if (keys.get(KeyEvent.VK_SPACE)) {
			p.jump();
		}
		
		if (keys.get(KeyEvent.VK_F5)) {
			map.nextMap();
			p.reset();
		}
	}

	private void onTimer() {
		map.onTimer();
	}
	
	@Override
	public void keyPressed(KeyEvent e) {
		// System.out.println(e);
		keys.put(e.getKeyCode(), Boolean.TRUE);
	}

	@Override
	public void keyReleased(KeyEvent e) {
		keys.put(e.getKeyCode(), Boolean.FALSE);
	}

	@Override
	public void keyTyped(KeyEvent e) {
		// System.out.println(e);
	}
}
