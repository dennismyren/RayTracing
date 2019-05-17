
#include "Sphere.h"


bool Sphere::hit(const Ray & r, HitRec & rec) const {
	float t = (c - r.o).dot(r.d);
	Vec3f p = r.o + r.d*t;
	float y = (c - p).len();
	float radSq = this->r * this->r;
	if (this->r < y || t < 0) {
		return false;
	}

	float dist = sqrt(radSq - y * y);
	float t1 = t - dist;
	float t2 = t + dist;
	float t0 = t1 < t2 ? t1 : t2;

	if (!rec.tHit || rec.tHit > t0) {
		rec.tHit = t1 < t2 ? t1 : t2;
		rec.anyHit = true;
		return true;
	}

	return false;


	/*
	Vec3f p = c - r.o;
	Vec3f d = r.d;
	float rad = this->r;

	float a = d.lenSq();
	float b = 2 * p.dot(d);
	float c = p.lenSq() - rad * rad;
	float t1 = (-b + sqrtf(b*b - 4 * a*c)) / (2 * a);
	float t2 = (-b - sqrtf(b*b - 4 * a*c)) / (2 * a);
	float t = t1 < t2 ? t1 : t2;

	if(isnan(t) || t > rec.tHit)
	{
		return false;
	}*/
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
	rec.col = col;
}
