#pragma once

#include "StdAfx.h"

namespace input
{
	extern vector<bool> keys;
	void mouseDown(MouseEvent e);
	void mouseUp(MouseEvent e);
	void mouseMove(MouseEvent e);
	void mouseDrag(MouseEvent e);
	void keyDown(KeyEvent e);
	void keyUp(KeyEvent e);
	void update();
}
