#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vec3.h"

class Camera {
public:
	Vec3f pos;

	Camera(Vec3f p) : pos(p) {}
	Camera(float x, float y, float z) : pos(Vec3f(x, y, z)) {}

	void set_pos(Vec3f p) { set_pos(p.x, p.y, p.z); }
	void set_pos(float x, float y, float z) { pos.x = x; pos.y = y; pos.z = z; }

	void add_pos_x(float x) { pos.x += x; }
	void add_pos_y(float y) { pos.y += y; }
	void add_pos_z(float z) { pos.z += z; }
};

#endif