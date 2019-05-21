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
	float opacity;
	float refractiveIndex;
public:
	Sphere(const Vec3f & cen, float rad, const Vec3f & color, float refl, float lref, float opac, float ri) : c(cen), r(rad), col(color), reflection(refl/(refl+lref+opac)), lightReflection(lref / (refl + lref + opac)), opacity(opac / (refl + lref + opac)), refractiveIndex(ri) { }
	
	bool hit(const Ray & r, HitRec & rec) const;
	void computeSurfaceHitFields(const Ray & r, HitRec & rec) const;

};

#endif