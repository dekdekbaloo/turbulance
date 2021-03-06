#ifndef KERNELH
#define KERNELH
#include "algebra3.h"
#include <GL/glut.h>
#include <cmath>
#include <stdio.h>

#define REST_DENSITY 1000.0f // 1000 kg/m^3
#define H 0.6f // smoothing radius
#define POW_H_9 (float)(H*H*H*H*H*H*H*H*H) // h^9
#define POW_H_6 (float)(H*H*H*H*H*H) // h^6
#define POW_H_3 (float)(H*H*H) // h^3
#define POW_H_5 (float)(H*H*H*H*H) // h^5
#define PI 3.141592653589

float wPoly6Kernel(vec3 ri, vec3 rj){
    vec3 rv = ri-rj ;
	float r = rv.length();
	float hr_term = (H * H - r * r);
	float div = 64.0 * PI * POW_H_9;
	if ( 0 <= r && r <= H)
        return 315.0f / div * hr_term * hr_term * hr_term ;
    else
        return 0.0f ;
}

vec3 wGradientPoly6Kernel(vec3 ri, vec3 rj){
    vec3 rv = ri-rj ;
	float r = rv.length();
	float coefficient = -945.0/(32.0*PI*POW_H_9);
	float hr_term = H*H - r * r ;
	if ( 0 <= r && r <= H)
        return coefficient*hr_term*hr_term*rv ;
    else
        return vec3(0.0f,0.0f,0.0f) ;
}
float wGradient2Poly6Kernel(vec3 ri, vec3 rj){
    vec3 rv = ri-rj ;
	float r = rv.length();
	float coefficient = -945.0/(32.0*PI*POW_H_9);
	float hr_term = H*H - r * r ;
	if ( 0 <= r && r <= H)
        return coefficient*hr_term*(3*H*H - 7*r) ;
    else
        return 0.0f ;
}

vec3 wGradient2SpikyKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length() ;
	if (r < 0.000001f) return  vec3(0.0f,0.0f,0.0f);
	float hr_term = H - r;
	float gradient_magnitude = -90.0f / (PI * POW_H_6) * hr_term;
	float div = (rv.length() + 0.001f);
	if ( 0 <= r && r <= H)
        return ( -gradient_magnitude)* rv * 1.0f / div ;
    else
        return vec3(0.0f,0.0f,0.0f) ;
}
vec3 wGradientSpikyKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length() ;
	if (r < 0.000001f) return  vec3(0.0f,0.0f,0.0f);
	float hr_term = H - r;
	float gradient_magnitude = 45.0f / (PI * POW_H_6) * hr_term * hr_term;
	float div = (rv.length() + 0.001f);
	if ( 0 <= r && r <= H)
        return ( -gradient_magnitude* 1.0f / div )* rv;
    else
        return vec3(0.0f,0.0f,0.0f) ;
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
        return vec3(0.0f,0.0f,0.0f) ;
}

float wGradient2ViscosityKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length() ;
	if (r < 0.000001f) return  0.0f;
	//float hr_term = 1-r/H ;
	float coefficient = 45.0f / ( PI * POW_H_6) ;
	//float div = (rv.length() + 0.001f);
	if ( 0 <= r && r <= H)
        return coefficient * (H-r);
    else
        return 0.0f ;
}


#endif
