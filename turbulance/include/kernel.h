#ifndef KERNELH
#define KERNELH
#include "algebra3.h"
#include <GL/glut.h>
#include <cmath>
#include <stdio.h>

#define REST_DENSITY 10000.0f // 1000 kg/m^3
#define H 0.1f // smoothing radius
#define POW_H_9 (float)(H*H*H*H*H*H*H*H*H) // h^9
#define POW_H_6 (float)(H*H*H*H*H*H) // h^6
#define POW_H_3 (float)(H*H*H) // h^3
#define POW_H_5 (float)(H*H*H*H*H) // h^5
#define PI 3.141592653589

vec3 wGradientPoly6Kernel(vec3 ri, vec3 rj){
    vec3 rv = ri-rj ;
	float r = rv.length();
	float hr_term = (H * H - r * r);
	float div = 64.0 * PI * POW_H_9;
	if ( 0 <= r && r <= H)
        return 315.0f / div * 6 * r * hr_term * hr_term * rv ;
    else
        return vec3(0.0f,0.0f,0.0f) ;
}

vec3 wGradientSpikyKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	//printf("ri : %f %f %f \n",ri.x,ri.y,ri.z);
	//printf("rj : %f %f %f \n",rj.x,rj.y,rj.z);
	//printf("rv : %f %f %f \n",rv.x,rv.y,rv.z);
	float r = rv.length() ;
	if (r < 0.000001f) return  vec3(0.0f,0.0f,0.0f);
	float hr_term = H - r;
	float gradient_magnitude = 45.0f / (PI * POW_H_6) * hr_term * hr_term;
	float div = (rv.length() + 0.001f);
   // printf("r : %f hr_term : %f gm : %f \n",r,hr_term,gradient_magnitude);
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

vec3 wGradient2ViscosityKernel(vec3 ri, vec3 rj){
	vec3 rv = ri-rj ;
	float r = rv.length() ;
	if (r < 0.000001f) return  vec3(0.0f,0.0f,0.0f);
	float hr_term = 1-r/H ;
	float gradient_magnitude = 45.0f / ( PI * POW_H_5) * hr_term ;
	float div = (rv.length() + 0.001f);
    //printf("r : %f hr_term : %f gm : %f \n",r,hr_term,gradient_magnitude);
	if ( 0 <= r && r <= H)
        return ( gradient_magnitude * 1.0f / div )* rv;
    else
        return vec3(0.0f,0.0f,0.0f) ;
}


#endif
