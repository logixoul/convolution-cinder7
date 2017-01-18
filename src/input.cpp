#include "StdAfx.h"
#include "input.h"
#include "global.h"
#include "Dragging.h"
#include "getOpt.h"
#include "colorspaces.h"

namespace input
{
	vector<bool> keys(256);
	void onDrag(Vec2i pos);
	static Dragging dragging([&](Vec2i v){ input::onDrag(v); });
	
	void mouseDown(MouseEvent e) { dragging.down(e); }
	void mouseUp(MouseEvent e) { dragging.up(e); }
	void mouseMove(MouseEvent e) { dragging.move(e); }
	void mouseDrag(MouseEvent e) { dragging.move(e); }
	void keyDown(KeyEvent e)
	{
		keys[e.getChar()]=true;
		if(e.getChar() == 'd')
		{
			GetOpt::params->isVisible() ? GetOpt::params->hide() : GetOpt::params->show();
		}
		if(e.getChar()=='[')zoom*=1.1;
		if(e.getChar()==']')zoom/=1.1;
	}
	void keyUp(KeyEvent e)
	{
		keys[e.getChar()]=false;
	}
	void update()
	{
		dragging.update();
	}

	void onDrag(Vec2i pos)
	{
		int radius = 5;
		static float hue = 0;
		Vec2f oldObjPos = Vec2f(dragging.lastPos)/windowScale;
		Vec2f newObjPos = Vec2f(AppBasic::get()->getMousePos())/windowScale;
		float step = (oldObjPos==newObjPos) ? 1 : 1/oldObjPos.distance(newObjPos);
		for(float i = 0; i <= 1; i+=step)
		{
			hue = fmod(hue + 0.02 * step, 1);
			auto delta = oldObjPos-newObjPos;
			//hue = (pi+atan2(delta.y, delta.x)) / twoPi;

			Vec2f objPos = lerp(oldObjPos, newObjPos, i);
			for(int x = objPos.x - radius; x <= objPos.x + radius; x++)
			{
				for(int y = objPos.y - radius; y <= objPos.y + radius; y++)
				{
					if(image.offsetOf(x, y) < image.offsetOf(0, 0) || image.offsetOf(x, y) > image.offsetOf(image.w-1, image.h-1))
						continue;
					
					Vec2f p(x, y);
					
					float value = 1-smoothstep(radius-1.f/10, (float)radius, Vec2f(x, y).distance(objPos));
					value = std::max(0.f, value);
					//image(x, y) = lerp(image(x, y), (Vec3f&)FromHSL(HslF(hue, 1, .5f)), value);
					float weight = 1/(Vec2f(x, y).distance(objPos)+.01);
					auto add = (Vec3f&)FromHSL(HslF(hue, 1, .5f))*weight;
					if(dragging.isDown[Left])image(x, y) += add;
					else
					{
						auto mul = (Vec3f&)FromHSL(HslF(hue, .2, .5f))/(Vec2f(x, y).distance(objPos)+.01);
						//image(x, y) -= apply(mul, boost::bind(powf, ::_1, Vec2f(x, y).distance(objPos)<2?1:0));
					}
				}
			}
		}
	}
}