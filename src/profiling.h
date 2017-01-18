#pragma once
#include "StdAfx.h"

namespace Pfl
{
	string getText();
	void nextFrame();

	struct sw {
		operator bool() { return false; }
		int ticks; string name; bool discarded;
		sw(string name);
		sw(sw& other);
		~sw();
	};
};
#define PFL(name) if(Pfl::sw s=Pfl::sw(#name)){}else
