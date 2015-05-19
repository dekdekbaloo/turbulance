#ifndef KERNELH
#define KERNELH
#include "algebra3.h"
#include <GL/glut.h>
#include <cmath>

#define REST_DENSITY 10000.0f // 1000 kg/m^3
#define H 1.5f // smoothing radius
#define POW_H_9 (float)(H*H*H*H*H*H*H*H*H) // h^9
#define POW_H_6 (float)(H*H*H*H*H*H) // h^6
#define POW_H_3 (float)(H*H*H) // h^6
#define PI 3.141592653589

vec3 wGradientPoly6Kernel(vec3 ri, vec3 rj){
    vec3 rv = ri-rj ;
	float r = rv.length();
	float hr_term = (H * H - r * r);
	float div = 64.0 * PI * POW_H_9;
	if ( 0 <= r && r <= H)
        return 315.0f / div * 6 * r * hr_term * hr_term * rv ;
    else
        return 0.0f ;
}

vec3 wGradientSpikyKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length();
	float hr_term = H - rv.length();
	float gradient_magnitude = 45.0f / (PI * POW_H_6) * hr_term * hr_term;
	float div = (rv.length() + 0.001f);
	if ( 0 <= r && r <= H)
        return ( gradient_magnitude * 1.0f / div )* rv;
    else
        return 0.0f ;
}

vec3 wGradientViscosityKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length() ;
	float hr_term = - (3*r*r)/(2*POW_H_3) + (2*r)/(H*H) - H / (2*r*r) ;
	float gradient_magnitude = 15.0f / (2* PI * POW_H_3) * hr_term ;
	float div = (rv.length() + 0.001f);
	if ( 0 <= r && r <= H)
        return ( gradient_magnitude * 1.0f / div )* rv;
    else
        return 0.0f ;
}

vec3 wGradient2ViscosityKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length() ;
	float hr_term = - (6*r)/(2*POW_H_3) + (2)/(H*H) + H / (r*r*r) ;
	float gradient_magnitude = 15.0f / (2* PI * POW_H_3) * hr_term ;
	float div = (rv.length() + 0.001f);
	if ( 0 <= r && r <= H)
        return ( gradient_magnitude * 1.0f / div )* rv;
    else
        return 0.0f ;
}


#endif
