#include "stdafx.h"
#include "Dragging.h"

using namespace cinder;
using namespace ci::app;
using namespace std;

// copied from GetTinderBoxWorking
Dragging::Dragging(function<void(Vec2i)> onDrag)
{
	this->onDrag = onDrag;
}
	
void Dragging::update()
{
	if(active) onDrag(Vec2i::zero());
}

bool Dragging::down(MouseEvent e)
{
	if(e.isLeft()) isDown[Left]=true;
	if(e.isRight()) isDown[Right]=true;
	active = true;
	lastPos = e.getPos();
	return true;
}

bool Dragging::up(MouseEvent e)
{
	if(e.isLeft()) isDown[Left]=false;
	if(e.isRight()) isDown[Right]=false;
	active = false;
	return true;
}

bool Dragging::move(MouseEvent e)
{
	if(e.isLeftDown())
	{
		onDrag(e.getPos() - lastPos);
	}
	lastPos = e.getPos();
	return true;
}
