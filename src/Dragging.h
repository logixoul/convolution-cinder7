#pragma once
#include "StdAfx.h"
using namespace ci;

enum MouseButton { Left, Right };

struct Dragging
{
	function<void(ivec2)> onDrag;
	Dragging(function<void(ivec2)> onDrag);
	bool active; 
	
	ivec2 lastPos;
public:
	map<MouseButton, bool> isDown;
	void update();
	bool down(app::MouseEvent e);
	bool up(app::MouseEvent e);
	bool move(app::MouseEvent e);
};
