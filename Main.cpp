#include<vector>
#include<iostream>
#include<limits>
#include <algorithm>
#include <math.h>
using namespace std;

#include "glut.h"

#include "Vec3.h"
#include "Image.h"
#include "Ray.h"
#include "Sphere.h"

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
	SimpleRayTracer(Scene * scene, Image * image) {
		this->scene = scene;
		this->image = image;
	}

	bool searchClosestHit(const Ray & ray, HitRec & hitRec) {
		for (int i = 0; i < scene->spheres.size(); i++) {
			if (scene->spheres[i].hit(ray, hitRec)) {
				hitRec.primIndex = i;
			}

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

	Vec3f calcFireRay(const Ray & ray, HitRec & hitRec)
	{
		Ray shadowRay;
		HitRec sHitRec;
		Vec3f col;
		if (searchClosestHit(ray, hitRec))
		{
			shadowRay.o = hitRec.p;
			for (auto& light_source : scene->light_sources)
			{
				col = PhonAmbiCalc(hitRec, light_source);
				sHitRec.reset();
				shadowRay.d = (light_source.pos - shadowRay.o).getNormalized();
				if (!searchClosestHit(shadowRay, sHitRec))
				{
					col += PhonDiffCalc(shadowRay, hitRec, light_source);
					col += PhonSpecCalc(ray, shadowRay, hitRec, light_source);
				}
				else
				{
					//col = Vec3f(0, 1, 0);
				}
			}
		}
		return col;
	}

	Vec3f PhonAmbiCalc(HitRec hr, LightSource ls) {
		float ambientStrength = 0.5;
		Vec3f ambient = hr.col * ls.col * ambientStrength;
		return ambient;
	}

	Vec3f PhonDiffCalc(Ray shadow, HitRec hr, LightSource ls) {
		Vec3f norm = hr.n.getNormalized();
		Vec3f lightDir = shadow.d.getNormalized();
		float diff = new_max(norm.dot(lightDir), 0.0);
		Vec3f diffuse = hr.col * ls.col * diff;

		return diffuse;
	}

	Vec3f PhonSpecCalc(Ray eye, Ray shadow, HitRec hr, LightSource ls) {
		float specularStrength = 1;
		float shineCoeff = 128;

		Vec3f lightDir = shadow.d.getNormalized();
		Vec3f viewDir = (eye.o - shadow.o).getNormalized();
		Vec3f reflectDir = (-lightDir).reflect(hr.n);
		float spec = pow(new_max(viewDir.dot(reflectDir), 0.0), shineCoeff);
		Vec3f specular = hr.col * ls.col * specularStrength * spec;

		return specular;
	}

	Vec3f phongCalc(Ray eye, Ray shadow, HitRec hr, LightSource ls)
	{
		float ambientStrength = 0.5;
		float specularStrength = 1;
		float shineCoeff = 128;
		Vec3f ambient = ls.col * ambientStrength;

		Vec3f norm = hr.n.getNormalized();
		Vec3f lightDir = shadow.d.getNormalized();
		float diff = new_max(norm.dot(lightDir), 0.0);
		Vec3f diffuse = ls.col * diff;

		Vec3f viewDir = (eye.o - shadow.o).getNormalized();
		Vec3f reflectDir = (-lightDir).reflect(hr.n);
		float spec = pow(new_max(viewDir.dot(reflectDir), 0.0), shineCoeff);
		Vec3f specular = ls.col * specularStrength * spec;

		Vec3f result = (ambient + diffuse + specular) * hr.col;

		return result;
		//float am_str = 0.1;*/
		//float shineCoef = 128;
		//float specularStrength = 0.5;

		//Vec3f lightDir = (ls.pos - shadow.o).getNormalized();
		////Vec3f ambient = (ls.col.cross(hr.col)) * am_str;
		//Vec3f diffuse = (ls.col.cross(hr.col)) * (new_max(hr.n.dot(lightDir), 0.0f));

		//Vec3f viewDir = (eye.o - shadow.o).getNormalized();
		//Vec3f reflectDir = (-lightDir).reflect(hr.n);

		//Vec3f spec = ls.col.cross(hr.col) * pow(new_max(viewDir.dot(reflectDir),0.0f),shineCoef);

		return result;
	}





	void fireRays(void) {
		Ray ray;
		HitRec hitRec;
		//bool hit = false;
		ray.o = Vec3f(0.0f, 0.0f, 0.0f); //Set the start position of the eye rays to the origin
		Vec3f col;
		for (int y = 0; y < image->getHeight(); y++) {
			for (int x = 0; x < image->getWidth(); x++) {
				ray.d = getEyeRayDirection(x, y);
				hitRec.reset();
				col = calcFireRay(ray, hitRec);
				setPixel(x, y, col);
			}
		}
	}
};


SimpleRayTracer * rayTracer;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	rayTracer->fireRays();


	glFlush();
}

void changeSize(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glViewport(0, 0, w, h);
}

void init(void)
{

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("SimpleRayTracer");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	//glutKeyboardFunc(keypress);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Scene * scene = new Scene;


	scene->add(Sphere(Vec3f(-5.0f, 0.0f, -20.0f), 3.0f, Vec3f(0.0f, 0.0f, 1.0f)));
	scene->add(Sphere(Vec3f(0.0f, 0.0f, -20.0f), 1.0f, Vec3f(1.0f, 0.0f, 0.0f)));
	scene->add(LightSource(Vec3f(0.0f, 0.0f, -1000.0f), Vec3f(1.0f, 1.0f, 1.0f)));
	Image * image = new Image(640, 480);

	rayTracer = new SimpleRayTracer(scene, image);

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	init();
	glutMainLoop();
}
