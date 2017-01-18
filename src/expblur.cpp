#include "StdAfx.h"
#include "expblur.h"
#include "D.h"

template<class Image>
void expTest(Image& src, float alpha)
{
	for(int x = 0; x < src.w; x++)
	{
		auto state = src(x, src.h-1);
		for(int y = 0; y < src.h; y++)
		{
			state = lerp(state, src(x, y), alpha);
			src(x, y) = state;
		}
		state = src(x, 0);
		for(int y = src.h-1; y >= 0; y--)
		{
			state = lerp(state, src(x, y), alpha);
			src(x, y) = state;
		}
	}
}

template<class Image>
void expTest2(Image& src, float const& alpha)
{
	for(int x = 0; x < src.w; x++)
	{
		int step = src.yStep();
		Image::value_type* end;
			
		auto state = src(x, src.h-1);
		end = &src(x, src.h-1);
		for(Image::value_type* p = &src(x, 0); p <= end;)
		{
			auto diff = *p - state;
			diff *= alpha;
			state += diff;
			//state = (Image::value_type&)_mm_add_ps((__m128&)state, (__m128&)diff);
			*p = state;
				p += step;
		}

		state = src(x, 0);
		end = &src(x, 0);
		for(Image::value_type* p = &src(x, src.h-1); p >= end;)
		{
			*p = state += alpha * (*p - state);
			p -= step;
		}
	}
}

template<class Image>
void expTest2D(Image& src, float alpha)
{
	expTest2(src, alpha);
	swap(src.w,src.h);
	expTest2((Array2D<Image::value_type, YSequential>&)src, alpha);
	swap(src.w,src.h);
}

template void expTest2D<::Image>(::Image& src, float alpha);
