#ifndef PARTICLE_H
#define PARTICLE_H
#include "algebra3.h"
#include <GL/glut.h>

class Particle
{
    private:

    public:
        vec3 r;
        vec3 v;
        float m;
        Particle(vec3 position,float mass);
        virtual ~Particle();
        void draw(){
            glColor3d(0,0.5f,0.5f);
            glPushMatrix();
                glTranslated(r.x,r.y,r.z);
                glutSolidSphere(0.1,16,10);
            glPopMatrix();
        }
        void update(int dt){
            r+=v*dt;
        }

};

#endif // PARTICLE_H
