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
        vec3 v;
        float m;
        float density = WATER_DENSITY ;
        pair<int,int> gridPos;
        Particle(vec3 position,float mass) : r(position) , m(mass) {}
        //virtual ~Particle();
        void draw(){
            glColor3d(0,0.5f,0.5f);

            glPushMatrix();
                glTranslated(r.x,r.y,r.z);
                glutSolidSphere(0.1f,10,10);
            glPopMatrix();
        }
        void update(int dt){
            r+=v*dt/1000;
        }

};

#endif // PARTICLE_H
