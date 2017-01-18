#include "StdAfx.h"
#include "misc.h"

Vec3f& fetch(Array2D<Vec3f>& src, Vec2i const& pos)
{
    static Vec3f black(0.0, 0.0, 0.0);
    if(pos.x < 0 || pos.y < 0 || pos.x >= src.w || pos.y >= src.h) return black;
	return src(pos);
}

void aaPoint(Array2D<Vec3f>& dest, Vec2f const& pos, Vec3f const& c) {
	int x = (int)pos.x;
	int y = (int)pos.y;
	float u_ratio = pos.x - x;
	float v_ratio = pos.y - y;

	if(x<0||y<0 || x>=dest.w-1 || y>=dest.h-1)
		return;
	float uv = u_ratio * v_ratio;
	float Uv = v_ratio - uv; // ((1-uratio) * v_ratio)
	float uV = u_ratio - uv; // ((1-vratio) * u_ratio)
	float UV = 1 - u_ratio - v_ratio + uv; // (1-uratio) * (1-vratio)
	auto addr = &dest(x, y);
	addr[0] += UV * c;
	addr[1] += uV * c;
	addr[dest.w] += Uv * c;
	addr[dest.w + 1] += uv * c;
}

Matrix33f toHsv;
Matrix33f toHsvInv;
int initHsv()
{
	toHsv = Matrix33f::createRotation(Vec3f(-1,1,0).normalized(), -acos(Vec3f(1,1,1).normalized().dot(Vec3f(0,0,1))));
	toHsvInv = toHsv.inverted();
	return 0;
}

static int ______ = initHsv();

// untested, fast. ok, now tested, seems to work.
float getSaturation(Vec3f const& v)
{
	// http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
	static const Vec3f mul = -Vec3f(1, 1, 1).normalized();
	return v.cross(mul).length();
}

// from the matrix.rotate method with hardcoded axis (1, 1, 1)
void rotateHue_ip( Vec3f& v, float angle )
{
	typedef float T;
	T sina = math<T>::sin(angle);
	T cosa1 = 1.0f - math<T>::cos(angle);
	T m = cosa1 - sina;
	T p = cosa1 + sina;

	T rx = v.x + m * v.y + p * v.z;
	T ry = p * v.x + v.y + m * v.z;
	T rz = m * v.x + p * v.y + v.z;

	v.x = rx;
	v.y = ry;
	v.z = rz;
}