#ifndef PLANE_H
#define PLANE_H
#include "algebra3.h"
#include <GL/glut.h>

class Plane
{
    private:
        vec3 n;
    public:
        Plane();
        virtual ~Plane();
        void draw(){
            glColor3f(1,1,1);
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
	//Trapezoid
	glNormal3f(n.x,n.y,n.z);
	glVertex3f(-50, -1.5f, 50.0f);
	glVertex3f(50, -1.5f, 50.0f);
	glVertex3f(50, -1.5f, -50.0f);
	glVertex3f(-50, -1.5f, -50.0f);
	glEnd(); //End quadrilateral coordinates


        }

};

#endif // PLANE_H
