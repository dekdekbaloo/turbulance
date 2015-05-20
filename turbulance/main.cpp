/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#include <windows.h>
#include <GL/glut.h>
#include "include/Plane.h"
#include "include/Particle.h"
#include "include/Kernel.h"
#include <vector>
#include <stdlib.h>
#include <map>
#include <set>
#include <math.h>
#include <stdio.h>

// #define W_VISCOSITY_CONT 0.000894f
#define W_VISCOSITY_CONT 4.0f
#define SCALE 0.00001f
#define K_GAS 0.82057f
#define K_TENSION 0.4f

static int gridSize = 0.5;
static float GRAVITY = -2.5f ;
static int numBall = 400 ;

int lastTime=0;
static float dt;
static vector<Particle> P;
Plane plane;
static map< pair<int,int> , set<int> > atomGridData;


static void printVec3(vec3 a , const char *string)
{
    printf (string);
   printf(" : %f %f %f \n",a.x,a.y,a.z);
}
static void init(){
    // Create Particle
    for(int i=0;i<numBall;i++){
        int kx = rand()%500 ;
        int ky = rand()%200 ;
        int kz = rand()%200 ;
        Particle part(vec3 (0.5+0.003*kx,0.1+0.02*ky,6-0.01*kz),1.0) ;
        //Particle part(vec3 (-2,0,-6),1) ;
        //part.v.z = rand()%100/100000.0 ;
        part.v = vec3 (0,0,0);

        P.push_back(part);
    }
}


/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}
//Calculate Force
/*
//Old Surface
static vec3 calculateTension(Particle p ,int index){
    vec3 term1(0.0f,0.0f,0.0f);
    vec3 term2(0.0f,0.0f,0.0f);
    for(int i=0;i<P.size();i++)
    {
        if(i == index) continue ;
        term1 += (P[i].m / P[i].density) * wGradient2SpikyKernel(p.r,P[i].r);
        term2 += (P[i].m / P[i].density) * wGradientSpikyKernel(p.r,P[i].r);
    }
    printVec3(term1,"Term1");
    printVec3(term2,"Term2");
    return (-K_TENSION * term1.length() / (term2.length()+ 0.001f)) * term2 ;
}*/
static vec3 calculateTension(Particle p ,int index){
    vec3 wr(0.0f,0.0f,0.0f);
    for(int i=0;i<P.size();i++)
    {
        if(i == index) continue ;
        wr += P[i].m * wGradientSpikyKernel(p.r,P[i].r).length()*(p.r -P[i].r);
    }
    //printVec3(wr,"Wr");
    return (K_TENSION /p.m) *wr ;
}
static vec3 calculatePressureForce(Particle p , Particle q){
    float pressure = 2*K_GAS*q.density;
    vec3 f_pressure = q.m*(pressure)/(2*q.density) * wGradientSpikyKernel(p.r ,q.r) ;

    return f_pressure ;
}
static vec3 calculatePressureTotal(Particle p ,int index){
    vec3 total(0.0f,0.0f,0.0f) ;
    for(int i=0;i<P.size();i++){
        if(i == index) continue ;
        total +=  calculatePressureForce(p,P[i]);
    }
    return -total ;
}
static vec3 calculateViscosity(Particle p , Particle q){
    vec3 f_viscosity = q.m*(q.v-p.v)/q.density * wGradient2ViscosityKernel(p.r ,q.r) ;
    //printf("Viscosity : %f %f %f \n",f_viscosity.x,f_viscosity.y,f_viscosity.z);
    return f_viscosity ;
}
static vec3 calculateViscosityTotal(Particle p ,int index){
    vec3 total(0.0f,0.0f,0.0f) ;
    for(int i=0;i<P.size();i++){
        if(i == index) continue ;
        total +=  calculateViscosity(p,P[i]) ;
    }

    //printf("F Viscosity : %f %f %f \n",total.x,total.y,total.z);
    return W_VISCOSITY_CONT *total ;
}
static void update(){
    int currTime=glutGet(GLUT_ELAPSED_TIME);
    dt=(currTime-lastTime);
    lastTime=currTime;

    for(int i=0;i<P.size();i++){
        vec3 total_a(0.0f,0.0f,0.0f) ;
        vec3 f_viscosity ;
        vec3 f_pressure ;
        vec3 f_tension ;

        int gridX = P[i].gridPos.first;
        int gridY = P[i].gridPos.second;
        boolean run = true;
       while(run)
        {
            //SPH
            //Smoothing Kernel Wpoly6
            //Viscosity
        f_viscosity = calculateViscosityTotal(P[i] , i);
        f_pressure = calculatePressureTotal(P[i] , i);
        f_tension = calculateTension(P[i] , i);
        total_a = (f_viscosity + f_pressure + f_tension  ) / WATER_DENSITY ;
       // printVec3(f_tension,"Tension");
      //  printf("A : %f %f %f \n",total_a.x,total_a.y,total_a.z);
        break ;
        }

        // printf("index : %d acc = %.12f\n",i,f_viscosity.length());
        // printf("Acc x = %.5f , y = %.5f ,z = %.5f \n",total_a.x,total_a.y,total_a.z);
        P[i].v = P[i].v + total_a*dt/1000;
        P[i].v.y += GRAVITY*dt/1000;
        P[i].update(dt);

                //Collision checking
        if(P[i].r.y<=-1.4f){
            P[i].r.y=-1.4f;
            //P[i].v.y = 0;
            P[i].v.y=-0.2f*P[i].v.y;
        }
        if(P[i].r.x<=-1.0f){
            P[i].r.x=-1.0f;
            P[i].v.x=-0.9f*P[i].v.x;
        }
        if(P[i].r.x>=3.0f){
            P[i].r.x=3.0f;
            P[i].v.x=-0.9f*P[i].v.x;
        }

        if(P[i].r.z<=-6.0f){
            P[i].r.z=-6.0f;
            P[i].v.z=-0.9f*P[i].v.z;
        }
        if(P[i].r.z>=-4.0f){
            P[i].r.z=-4.0f;
            P[i].v.z=-0.9f*P[i].v.z;
        }
        //printf("Pos x = %.5f , y = %.5f ,z = %.5f ",P[i].r.x,P[i].r.y,P[i].r.z);
    }

    //for (int i = 0; i < )
}

static void display(void)
{
    update();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(int i=0;i<P.size();i++){
            P[i].draw();
        }
        plane.draw();

    glutSwapBuffers();

}


static void key(unsigned char key, int x, int y)
{
    glutPostRedisplay();
}

int c = 0 ;
static void idle(void)
{
    glutPostRedisplay();
    //c++ ;
    if (c == 5 ) {
        while(1) {

        }
    }
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(0,0,0,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);


    init();
    glutMainLoop();

    return EXIT_SUCCESS;
}
