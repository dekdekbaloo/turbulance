#include "Particle.h"

Particle::Particle(vec3 position,float mass)
{
    r=position;
    m=mass;
}

Particle::~Particle()
{
    //dtor
}
