#pragma once

#include "StdAfx.h"
#include "util.h"

Vec3f& fetch(Array2D<Vec3f>& src, Vec2i const& pos);
void aaPoint(Array2D<Vec3f>& dest, Vec2f const& pos, Vec3f const& c);

extern Matrix33f toHsv;
extern Matrix33f toHsvInv;

float getSaturation(Vec3f const& v);
void rotateHue_ip(Vec3f& v, float angle);

struct HsvStruct
{
	Vec2f transform(Vec3f const& v)
	{
		return Vec2f(toHsv.m00*v.x + toHsv.m01*v.y + toHsv.m02*v.z, toHsv.m10*v.x + toHsv.m11*v.y + toHsv.m12*v.z);
	}
};
