#include "StdAfx.h"
#include "init.h"
#include "global.h"
#include "getOpt.h"
#include "render.h"
#include "picopng.h"

void initApp()
{
	AllocConsole();
	std::fstream* fs = new std::fstream("CONOUT$");
	std::cout.rdbuf(fs->rdbuf());
		
	//bench();

	GetOpt::Init();

	setWindowSize(windowSize, windowSize);
	//setFullScreen(true);
		
	int g=1;
	for(int i=0;i<image.w;i+=g)for(int j=0;j<image.h;j+=g)
	{
		Vec3f r(randFloat(), randFloat(), randFloat());
		for(int x = i; x < std::min(i+g,image.w);x++)for(int y=j;y<std::min(j+g, image.h);y++)
			{
				image(x,y)=r;
			}
	}
		
	loadPNG("test.png", pic);
	render::init();
}
	