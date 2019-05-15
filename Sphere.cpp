
#include "Sphere.h"


bool Sphere::hit(const Ray & r, HitRec & rec) const {	
	
	Vec3f p =c - r.o;
	Vec3f d = r.d;
	float rad = this->r;

	float a = d.lenSq();
	float b = 2 * p.dot(d);
	float c = p.lenSq() - rad * rad;
	float t1 = (b + sqrtf(b*b - 4 * a*c)) / (2 * a);
	float t2 = (b - sqrtf(b*b - 4 * a*c)) / (2 * a);
	rec.tHit = t1 < t2 ? t1 : t2;
	if (isnan(rec.tHit)) {
		return false;
	}
	this->computeSurfaceHitFields(r, rec);
	rec.anyHit = true;
	return true;
	/*
	Vec3f v = c - r.o;
	float s = v.dot(r.d);
	float vLenSq = v.dot(v);
	float radSq = this->r * this->r; 
	if (s < 0 && vLenSq > radSq) {
		return false;
	}
	float mSq = vLenSq - s * s;
	if (mSq > radSq) {
		return false;
	}
	rec.anyHit = true;
	return true;
	*/
}


void Sphere::computeSurfaceHitFields(const Ray & r, HitRec & rec) const {
	rec.p = r.o + r.d * rec.tHit;
	rec.n = (rec.p - c).normalize();
}
