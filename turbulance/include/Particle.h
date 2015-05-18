#ifndef PARTICLE_H
#define PARTICLE_H
#include "algebra3.h"
#include <GL/glut.h>

class Particle
{
    private:
        vec3 r;
        float m;
        float v;
    public:
        Particle(vec3 position,float mass);
        virtual ~Particle();
        void draw(){
            glColor3d(0,0.5f,0.5f);
            glPushMatrix();
                glTranslated(r.x,r.y,r.z);
                glutSolidSphere(0.1,10,10);
            glPopMatrix();
        }

};

#endif // PARTICLE_H
