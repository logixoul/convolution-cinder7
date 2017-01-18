#include "StdAfx.h"

#include "profiling.h"
#include "global.h"

namespace Pfl
{
	const float smoothing = .01;
	static map<string, float> info;
	
	static float round(float d)
	{
	  return floor(d + 0.5f);
	}

	string getText()
	{
		stringstream text;
		/*text << "total: " <<
			boost::accumulate(
				info
				| boost::adaptors::transformed([&](map<string, float>::value_type& pair) { return pair.second; })
			);*/
		float sum = 0; foreach(auto pair, info) sum += pair.second;
		text << "total: " << (int)round(sum) << endl;
		foreach(auto& pair, info)
		{
			text << pair.first << " took " << (int)round(pair.second) << "ms" << endl;
		}
		return text.str();
	}
	void nextFrame() {
		foreach(auto& pair, info)
		{
			pair.second *= 1.0 - smoothing;
		}
	}
	sw::sw(string name) { ticks = clock(); this->name=name; discarded = false; }
	sw::sw(sw& other) { other.discarded = true; ticks = other.ticks; name = other.name; discarded = false; }
	sw::~sw() {
		if(!discarded) {
			if(framesElapsed == 0) // ignore lazy initialization stuff
				return;
			if(info.find(name) == info.end())
				info[name] = 0;
			info[name]+=(framesElapsed == 1 ? 1.0 : smoothing) * (clock()-ticks)*1000/CLOCKS_PER_SEC;
		}
	}
}