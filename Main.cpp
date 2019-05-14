#include<vector>
#include<iostream>
using namespace std;

#include "glut.h"

#include "Vec3.h"
#include "Image.h"
#include "Ray.h"
#include "Sphere.h"


class Scene {
public:
	vector<Sphere> spheres;		

	Scene(void) {
		 
	}
	void add(Sphere s) {
		spheres.push_back(s); 
		cout << "Sphere added: " << "r = " << spheres[spheres.size()-1].r << endl;
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
		static float dx =  sizeX / float(image->getWidth());  
		static float dy =  sizeY / float(image->getHeight());
	
		return Vec3f(left + x * dx, bottom + y * dy, z).normalize();
	}


public:
	SimpleRayTracer(Scene * scene, Image * image) {
		this->scene = scene;
		this->image = image;
	}

	void searchClosestHit(const Ray & ray, HitRec & hitRec) {
		for (int i = 0; i < scene->spheres.size(); i++) {
			scene->spheres[i].hit(ray, hitRec);
		}
	}

	void fireRays(void) { 
		Ray ray;
		HitRec hitRec;
		//bool hit = false;
		ray.o = Vec3f(0.0f, 0.0f, 0.0f); //Set the start position of the eye rays to the origin

		for (int y = 0; y < image->getHeight(); y++) {
			for (int x = 0; x < image->getWidth(); x++) {
				ray.d = getEyeRayDirection(x, y);
				hitRec.anyHit = false;
				searchClosestHit(ray, hitRec);
				if (hitRec.anyHit) {
					Vec3f vec = Vec3f(1.0f, 1.0f, 1.0f);
					image->setPixel(x, y, vec);
					glSetPixel(x, y, vec);
				} else {
					Vec3f vec = Vec3f(0.0f, 0.0f, 0.0f);
					image->setPixel(x, y, vec);
					glSetPixel(x, y, vec);
				}
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
	glViewport(0,0,w,h);
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
	scene->add(Sphere(Vec3f(0.0f, 0.0f, -10.0f), 3.0f));

	Image * image = new Image(640, 480);	
	
	rayTracer = new SimpleRayTracer(scene, image);

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	init();
	glutMainLoop();
}
