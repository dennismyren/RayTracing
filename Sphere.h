#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Vec3.h"
#include "Ray.h"

class Sphere {
public:
	Vec3f c;
	float r;
	Vec3f col;
	float reflection;
	float lightReflection;
public:
	Sphere(const Vec3f & cen, float rad, const Vec3f & color, float refl, float lref) : c(cen), r(rad), col(color), reflection(refl/(refl+lref)), lightReflection(lref / (refl + lref)) { }
	
	bool hit(const Ray & r, HitRec & rec) const;
	void computeSurfaceHitFields(const Ray & r, HitRec & rec) const;

};

#endif