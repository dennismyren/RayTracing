#ifndef _RAY_H_
#define _RAY_H_

#include "Vec3.h"
#include <limits>

class Ray {
public:
	Vec3f o, d;
	float tClip;	
	static float rayEps;
	static float tMax;
public:
	Ray() : tClip(tMax) { }	
	Ray(const Vec3f & o, const Vec3f& d, float tclip = tMax) : o(o), d(d), tClip(tMax) { }
	
	void epsMoveStartAlongSurfaceNormal(const Vec3f & n) { o = o + n * rayEps; }
	void epsMoveStartAlongDir() { o = o + d * rayEps; }
	
	static void incRayEps() { rayEps *= 10.0f; }
	static void decRayEps() { rayEps /= 10.0f; }
};

class LightSource{
public:

	LightSource(const Vec3f& pos, const Vec3f& col)
		: pos(pos),
		  col(col)
	{
	}

	Vec3f pos;
	Vec3f col;
};


class HitRec {
public:	
	float tHit;
	int primIndex;
	Vec3f col;
	Vec3f p;
	Vec3f n;
	Vec3f sp;
	bool anyHit;

	void reset()
	{
		tHit = INT_MAX;
		anyHit = false;
		primIndex = 0;
		col = Vec3f(0.0f, 0.0f, 0.0f);
		p = Vec3f(0.0f, 0.0f, 0.0f);
		n = Vec3f(0.0f, 0.0f, 0.0f);
	}
};


#endif
