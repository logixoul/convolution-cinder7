#include "StdAfx.h"
#include "input.h"
#include "global.h"
#include "Dragging.h"
#include "getOpt.h"
#include "colorspaces.h"

namespace input
{
	vector<bool> keys(256);
	void onDrag(ivec2 pos);
	static Dragging dragging([&](ivec2 v){ input::onDrag(v); });
	
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

	void onDrag(ivec2 pos)
	{
		int radius = 5;
		static float hue = 0;
		vec2 oldObjPos = vec2(dragging.lastPos)/windowScale;
		vec2 newObjPos = vec2(AppBase::get()->getMousePos())/windowScale;
		float step = (oldObjPos==newObjPos) ? 1 : 1/distance(oldObjPos, newObjPos);
		for(float i = 0; i <= 1; i+=step)
		{
			hue = fmod(hue + 0.02 * step, 1);
			auto delta = oldObjPos-newObjPos;
			//hue = (pi+atan2(delta.y, delta.x)) / twoPi;

			vec2 objPos = lerp(oldObjPos, newObjPos, i);
			for(int x = objPos.x - radius; x <= objPos.x + radius; x++)
			{
				for(int y = objPos.y - radius; y <= objPos.y + radius; y++)
				{
					if(image.offsetOf(x, y) < image.offsetOf(0, 0) || image.offsetOf(x, y) > image.offsetOf(image.w-1, image.h-1))
						continue;
					
					vec2 p(x, y);
					
					float value = 1-smoothstep(radius-1.f/10, (float)radius, distance(vec2(x, y), objPos));
					value = std::max(0.f, value);
					//image(x, y) = lerp(image(x, y), (vec3&)FromHSL(HslF(hue, 1, .5f)), value);
					float weight = 1/(distance(vec2(x, y), objPos)+.01);
					auto add = (vec3&)FromHSL(HslF(hue, 1, .5f))*weight;
					if(dragging.isDown[Left])image(x, y) += add;
					else
					{
						auto mul = (vec3&)FromHSL(HslF(hue, .2, .5f))/(distance(vec2(x, y), objPos)+.01f);
						//image(x, y) -= apply(mul, boost::bind(powf, ::_1, vec2(x, y).distance(objPos)<2?1:0));
					}
				}
			}
		}
	}
}