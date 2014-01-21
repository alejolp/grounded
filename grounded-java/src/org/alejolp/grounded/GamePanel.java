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

import java.awt.BorderLayout;
import java.awt.Toolkit;
import java.lang.reflect.InvocationTargetException;

import javax.swing.JPanel;
import javax.swing.SwingUtilities;

public class GamePanel extends JPanel implements Runnable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	private Thread t;
	private GameCanvas mc;
	
	private Toolkit tk;
	private Runnable updater;
	
	public GamePanel() {
		super();
		
		setLayout(new BorderLayout());
		validate();
		
		mc = new GameCanvas();
		removeAll();
		add(mc, BorderLayout.CENTER);
		mc.setVisible(true);
	}
	
	public void start() {
		mc.init();
		
		tk = getToolkit();
		updater = new Runnable() {
			@Override
			public void run() {
				// tk.sync();
				try {
					Game.getInstance().update();
				} catch (RuntimeException e) {
					e.printStackTrace();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};
		
		t = new Thread(this);
		t.start();
	}
	
	public void stop() {
		t.interrupt();
	}
	
	@Override
	public void run() {
		while (true) {
			mc.repaint();
			syncToolkit();
			
			try {
				Thread.sleep(1000/25);
			} catch (InterruptedException e) {
				System.err.println(e);
			}
		}
	}
	
	private void syncToolkit() {
		try {
			SwingUtilities.invokeAndWait(updater);
		} catch (InterruptedException e1) {
			System.err.println(e1);
			e1.printStackTrace();
		} catch (InvocationTargetException e1) {
			System.err.println(e1);
			e1.printStackTrace();
		}
	}
}
