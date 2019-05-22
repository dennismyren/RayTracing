
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
	float t0 = t1 > 0.0f && t1 < t2 ? t1 : t2;

	if (!rec.tHit || rec.tHit > t0) {
		rec.tHit = t0;
		rec.anyHit = true;
		return true;
	}

	return false;
}


void Sphere::computeSurfaceHitFields(const Ray & r, HitRec & rec) const {
	rec.p = r.o + r.d * rec.tHit;
	rec.n = (rec.p - c).normalize();
	rec.col = col;
}
