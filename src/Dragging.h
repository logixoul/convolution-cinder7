#pragma once
#include "StdAfx.h"
using namespace ci;

enum MouseButton { Left, Right };

struct Dragging
{
	function<void(Vec2i)> onDrag;
	Dragging(function<void(Vec2i)> onDrag);
	bool active; 
	
	Vec2i lastPos;
public:
	map<MouseButton, bool> isDown;
	void update();
	bool down(app::MouseEvent e);
	bool up(app::MouseEvent e);
	bool move(app::MouseEvent e);
};
