package org.alejolp.grounded;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JFrame;

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

public class MainApp extends JFrame {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	private GamePanel mp = new GamePanel();

	public MainApp() {
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		getContentPane().add(mp, BorderLayout.CENTER);
		mp.setPreferredSize(new Dimension(Constants.PANEL_WIDTH, Constants.PANEL_HEIGHT));
		pack();		
		setVisible(true);

		enableInputMethods(true);
		setFocusable(true);
		addKeyListener(Game.getInstance());
        requestFocusInWindow();
        mp.start();
	}

	public static void main(String[] args) {
		MainApp app = new MainApp();
	}

}
