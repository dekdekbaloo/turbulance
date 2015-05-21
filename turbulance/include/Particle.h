#ifndef PARTICLE_H
#define PARTICLE_H
#include "algebra3.h"
#include <GL/glut.h>

#define SCALE 0.000001f
#define WATER_DENSITY  998.8f
class Particle
{
    private:

    public:
        vec3 r;
        vec3 prev_r;
        vec3 v;
        float m = 1.0f;
        float density = WATER_DENSITY ;
        float nearDensity = WATER_DENSITY ;
        float P = WATER_DENSITY ;
        float nearP = WATER_DENSITY ;
        vec3 gridPos;
        Particle(vec3 position,float mass) : r(position) , m(mass) {}
        //virtual ~Particle();
        void draw(){
           glColor3d(0,0.5f-v.length()*0.1f,0.5f);

            glPushMatrix();
                glTranslated(r.x,r.y,r.z);
                glutSolidSphere(0.05f,10,10);
            glPopMatrix();
/*
            glPushMatrix();
            glPointSize(1.0f);

            glColor3f(v.x*2, v.y*2,v.z*5 );

            glBegin(GL_POINTS);
			glVertex3f(r.x,r.y,r.z);
            glEnd();
             glPopMatrix();
*/

        }

};

#endif // PARTICLE_H
