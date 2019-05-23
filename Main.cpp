#include<vector>
#include<iostream>
#include<limits>
#include <algorithm>
#include <math.h>
#include <chrono>
using namespace std;

#include "glut.h"

#include "Vec3.h"
#include "Image.h"
#include "Ray.h"
#include "Sphere.h"
#include "Camera.h"

#define new_max(x,y) ((x) >= (y)) ? (x) : (y)
#define new_min(x,y) ((x) <= (y)) ? (x) : (y)


class Scene {
public:
	vector<Sphere> spheres;
	vector<LightSource> light_sources;

	Scene(void) {

	}
	void add(Sphere s) {
		spheres.push_back(s);
		cout << "Sphere added: " << "r = " << spheres[spheres.size() - 1].r << endl;
	}

	void add(LightSource ls) {
		light_sources.push_back(ls);
		cout << "Light source added." << endl;
	}

	void load(char * fileName) {
		// load a file with spheres for your scene here ...
	}

};


void glSetPixel(int x, int y, Vec3f & c) {
	glColor3f(c.r, c.g, c.b);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

class SimpleRayTracer {
private:
	Scene * scene;
	Image * image;
	Camera cam = Camera(Vec3f(0.0f, 0.0f, 0.0f));
	int intersectionTestsCount;

	Vec3f getEyeRayDirection(int x, int y) {
		//Uses a fix camera looking along the negative z-axis
		static float z = -5.0f;
		static float sizeX = 4.0f;
		static float sizeY = 3.0f;
		static float left = -sizeX * 0.5f;
		static float bottom = -sizeY * 0.5f;
		static float dx = sizeX / float(image->getWidth());
		static float dy = sizeY / float(image->getHeight());

		return Vec3f(left + x * dx, bottom + y * dy, z).normalize();
	}


public:

	int getIntersectCount()
	{
		return intersectionTestsCount;
	}

	SimpleRayTracer(Scene * scene, Image * image) {
		this->scene = scene;
		this->image = image;
	}

	bool searchClosestHit(const Ray & ray, HitRec & hitRec) {
		hitRec.reset();

		for (int i = 0; i < scene->spheres.size(); i++) {
			if (scene->spheres[i].hit(ray, hitRec)) {
				hitRec.primIndex = i;
			}
			intersectionTestsCount++;
		}

		if (hitRec.anyHit) {
			scene->spheres[hitRec.primIndex].computeSurfaceHitFields(ray, hitRec);
		}

		return hitRec.anyHit;
	}

	void setPixel(int x, int y, Vec3f vec) const
	{
		image->setPixel(x, y, vec);
		glSetPixel(x, y, vec);
	}

	Vec3f calcFireRay(const Ray & ray, HitRec & hitRec, int jumpsLeft)
	{
		if (jumpsLeft < 0)
		{
			return Vec3f(0, 0, 0);
		}
		hitRec.reset();
		Ray shadowRay;
		Ray reflectionRay;
		HitRec sHitRec;
		Sphere *sphere;
		Vec3f lightCol = Vec3f(0, 0, 0);
		Vec3f reflectCol = Vec3f(0, 0, 0);

		if (searchClosestHit(ray, hitRec))
		{
			sphere = &scene->spheres[hitRec.primIndex];
			shadowRay.o = hitRec.p;

			reflectionRay.o = hitRec.p;
			reflectionRay.d = (-ray.d).reflect(hitRec.n);
			if (sphere->lightReflection > 0.01f)
			{
				for (auto& light_source : scene->light_sources)
				{
					lightCol += PhonAmbiCalc(light_source.col);
					for (auto& lp : light_source.points)
					{
						sHitRec.reset();
						shadowRay.d = (lp - shadowRay.o).getNormalized();
						if (!searchClosestHit(shadowRay, sHitRec))
						{
							lightCol += PhonDiffCalc(hitRec, light_source.col, lp) / light_source.size;
							lightCol += PhonSpecCalc(ray, hitRec, light_source.col, lp) / light_source.size;
						}
					}
				}
				lightCol *= sphere->col * sphere->lightReflection;
			}
			if (sphere->reflection > 0.01f)
			{
				reflectCol += calcFireRay(reflectionRay, hitRec, jumpsLeft - 1) * sphere->reflection;
			}
		}
		return (lightCol + reflectCol);
	}

	Vec3f PhonAmbiCalc(Vec3f col) {
		float ambientStrength = 0.5;
		Vec3f ambient = col * ambientStrength;
		return ambient;
	}

	Vec3f PhonDiffCalc(HitRec hr, Vec3f lcol, Vec3f lpos) {
		Vec3f norm = hr.n.getNormalized();
		Vec3f lightDir = (lpos - hr.p).getNormalized();
		float diff = new_max(norm.dot(lightDir), 0.0);
		Vec3f diffuse = lcol * diff;

		return diffuse;
	}

	Vec3f PhonSpecCalc(Ray eye, HitRec hr, Vec3f lcol, Vec3f lpos) {
		float specularStrength = 1.0f;
		float shineCoeff = 128;

		Vec3f lightDir = (lpos - hr.p).getNormalized();
		Vec3f viewDir = (eye.o - hr.p).getNormalized();
		Vec3f reflectDir = (lightDir).reflect(hr.n);
		float spec = pow(new_max(viewDir.dot(reflectDir), 0.0f), shineCoeff);
		Vec3f specular = lcol * specularStrength * spec;

		return specular;
	}

	void fireRays(void) {
		Ray ray;
		HitRec hitRec;
		int jumps = 3;
		ray.o = cam.pos; //Set the start position of the eye rays to the origin
		Vec3f col;
		for (int y = 0; y < image->getHeight(); y++) {
			for (int x = 0; x < image->getWidth(); x++) {
				ray.d = getEyeRayDirection(x, y);
				hitRec.reset();
				col = calcFireRay(ray, hitRec, jumps);
				setPixel(x, y, col);
			}
			if (y % 50 == 0) {
				cout << "Did line: " << y << " out of " << image->getHeight() << endl;
			}
		}
	}
};


SimpleRayTracer * rayTracer;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	time_t start = time(NULL);
	rayTracer->fireRays();
	time_t finish = time(NULL);
	cout << "Elapsed time : " << finish - start << endl;
	cout << "Intersection tests : " << rayTracer->getIntersectCount() << endl;
	glFlush();
}

void changeSize(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glViewport(0, 0, w, h);
}

std::vector<Vec3f> createVecorsAroundPoint(Vec3f p) {
	std::vector<Vec3f> points;
	float dist = 1;
	points.push_back(p);
	points.push_back(p + Vec3f(dist * -2, dist * 2, 0));
	points.push_back(p + Vec3f(dist * 0, dist * 2, 0));
	points.push_back(p + Vec3f(dist * 2, dist * 2, 0));
	points.push_back(p + Vec3f(dist * -1, 0, 0));
	points.push_back(p + Vec3f(dist * 1, 0, 0));
	points.push_back(p + Vec3f(0, dist * -2, 0));


	return points;
}

void init(void)
{
	int w = 640;
	int h = 480;
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(w, h);
	glutCreateWindow("SimpleRayTracer");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	//glutKeyboardFunc(keypress);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Scene * scene = new Scene;
	Vec3f point1 = Vec3f(-0.1f, 0.0f, 10.0f);
	Vec3f point2 = Vec3f(0.1f, 0.0f, 10.0f);
	Vec3f point3 = Vec3f(0.0f, -0.1f, 10.0f);

	scene->add(Sphere(Vec3f(-4.0f, 0.0f, -100.0f), 80.0f, Vec3f(0.0f, 0.0f, 1.0f), 0, 1));

	scene->add(Sphere(Vec3f(4.0f, 0.0f, -10.0f), 3.0f, Vec3f(0.0f, 0.0f, 1.0f), 1, 0));
	scene->add(Sphere(Vec3f(0.0f, 2.5f, -10.0f), 1.0f, Vec3f(1.0f, 1.0f, 0.0f), 0, 1));
	scene->add(Sphere(Vec3f(-4.0f, 0.0f, -10.0f), 3.0f, Vec3f(1.0f, 0.0f, 1.0f), 1, 1));
	scene->add(Sphere(Vec3f(-3.0f, 1.0f, -5.0f), 0.8f, Vec3f(1.0f, 1.0f, 1.0f), 0, 1));
	scene->add(Sphere(Vec3f(0.0f, -2.5f, -10.0f), 1.0f, Vec3f(0.0f, 1.0f, 1.0f), 0, 1));
	scene->add(LightSource(createVecorsAroundPoint(Vec3f(0, 0, 100)), Vec3f(0.5f, 0.5f, 0.5f)));
	Image * image = new Image(w, h);

	rayTracer = new SimpleRayTracer(scene, image);

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	init();
	glutMainLoop();
}
