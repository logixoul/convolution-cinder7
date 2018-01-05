#pragma once

#include "StdAfx.h"
#include "util.h"

vec3& fetch(Array2D<vec3>& src, ivec2 const& pos);
void aaPoint(Array2D<vec3>& dest, vec2 const& pos, vec3 const& c);

extern mat3 toHsv;
extern mat3 toHsvInv;

float getSaturation(vec3 const& v);
void rotateHue_ip(vec3& v, float angle);

/*struct HsvStruct
{
	vec2 transform(vec3 const& v)
	{
		return vec2(toHsv.m00*v.x + toHsv.m01*v.y + toHsv.m02*v.z, toHsv.m10*v.x + toHsv.m11*v.y + toHsv.m12*v.z);
	}
};
*/