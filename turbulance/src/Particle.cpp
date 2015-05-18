#include "Particle.h"

Particle::Particle(vec3 position,float mass)
{
    r=position;
    m=mass;
    v=vec3 (0,0,0);
}

Particle::~Particle()
{
    //dtor
}
