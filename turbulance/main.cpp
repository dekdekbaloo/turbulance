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
#define GLEW_STATIC
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <windows.h>
#include "include/Plane.h"
#include "include/Particle.h"
#include "include/Kernel.h"
#include "include/Collision.h"
#include <vector>
#include <stdlib.h>
#include <map>
#include <set>
#include <math.h>
#include <stdio.h>


// #define W_VISCOSITY_CONT 0.000894f
#define W_VISCOSITY_CONT 0.894f
#define K_GAS 8.314f

#define K_TENSION 0.00001f
#define GLEW_STATIC 50
#define PI 3.141592653589
#define NORM 20/(2*PI*H*H)
#define NEARNORM 30/(2*PI*H*H)
#define REST_DENSITY 989.0f
#define SURFACE_THRESHOLD 0.01f

static float gridSize = 0.2f;
static float GRAVITY = -9.8f ;
static int numBall = 200;
static float startX = -0.5f;;
static float sizeX = 1.0f;
static float startY = -1.4f;
static float sizeY = 100; //Not yet used
static float startZ = -6.0f;
static float sizeZ = 1.0f;
static float c_rad = 4.0f;

static Collision coll;

struct CompareVectors
{
    bool operator()(const vec3& a, const vec3& b)
    {
        return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
    }
};

typedef map<vec3, vector<int>, CompareVectors> VectorMap;

float deltaAngle = 0.0f;
int buttonState=0;
int xOrigin = -1;

int lastTime=0;
int test = 0 ;
static float dt;
static vector<Particle> P;
Plane plane;
static VectorMap gridMap;

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
        int kz = rand()%500 ;
        //Particle part(vec3 (-1,5,1) ,1.0f);
        Particle part(vec3 (0.5-0.01*kx,0.1-0.01*ky,-4-0.01*kz),1);

        //Particle part(vec3 (-2,0,-6),1) ;
        part.v = vec3 (0,0,0);
       /*part.v.z = rand()%100/100.0 ;
       part.v.y = rand()%100/100.0 ;
       part.v.x = rand()%100/100.0 ;*/
        vec3 newGridPos = part.r/gridSize;
        gridMap[newGridPos].push_back(i);
        P.push_back(part);
    }
}


/* GLUT callback Handlers */
float lx=0.0f;
float lz=0.0f;
float angle=0.0f;
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//Calculate Force

void calculatePressure()
{
        //iterate over neighbor grid cell!
        for (size_t i = 0 ; i < P.size() ; i++){

            float density = 989.0f;
            float nearDensity = 0;
            Particle &pi = P[i] ;
            int co = 0 ;
            int gridX = P[i].gridPos.x;
            int gridY = P[i].gridPos.y;
            int gridZ = P[i].gridPos.z;
            /*
            // Grid Methods
            for(int a = -1; a < 2; a++)
            {
                gridX = gridX + a;
                if (gridX < 0) continue;
                for (int b = -1; b < 2; b++)
                {
                    gridY = gridY + b;
                    if (gridY < 0) continue;
                    for (int c = -1; c < 2; c++)
                    {
                        gridZ = gridZ + c;
                        if (gridZ < 0) continue;
                        vec3 gridKey = vec3(gridX,gridY,gridZ);

                        for (vector<int>::iterator it = gridMap[gridKey].begin(); it != gridMap[gridKey].end(); it++)
                        {
                            if(i != *it){
                                const Particle &pj = P[*it];
                                vec3 dis = pi.r - pj.r ;
                                co++;
                                //printf("part: %d , part: %d , dis = %.5f \n",i,*it,dis.length());
                                density += wPoly6Kernel(pi.r , pj.r);
                            }
                        }
                    }
                }
            }*/
            //Brute Force
            for (size_t j = 0 ; j < P.size() ; j++){
                if (i == j) continue ;

                Particle &pj = P[j];
                vec3 dis = pi.r - pj.r ;
                if (dis.length() > H*H) continue ;
                co++;
                //printVec3(pi.r ,"Pi.r ");
                //printVec3(pj.r ,"Pj.r ");
                density += wPoly6Kernel(pi.r , pj.r);
            }
        pi.density = density;
        //printf("part: %d , total n: %d , dens = %.5f \n",i,co,density);
        //pi.nearDensity = nearDensity;
        pi.P = K_GAS * (density - REST_DENSITY);
        //pi.nearP = K_GAS_NEAR * nearDensity;
        //printf("Particle %d Neighbor =%d :Density=%.6f P=%.6f\n",i,c,pi.density,pi.P);
    }
}
static void checkCollision(Particle &p)
{
    //Box Collision checking
        if(p.r.y<= startY){
            p.r.y= startY;
            //p.v.y = 0;
            p.v.y= -0.4f*p.v.y;

        }
        if(p.r.x<= startX){
            p.r.x= startX;
            p.v.x=-0.9f*p.v.x;
        }
        if(p.r.x>= startX+sizeX){
            p.r.x= startX+sizeX;
            p.v.x=-0.9f*p.v.x;
        }
        if(p.r.z<= startZ){
            p.r.z= startZ;
            p.v.z=-0.9f*p.v.z;
        }
        if(p.r.z>= startZ+sizeZ){
            p.r.z= startZ+sizeZ;
            p.v.z=-0.9f*p.v.z;
        }
    //Cylinder Collision checking
       /*float xz = p.r.x*p.r.x + p.r.z*p.r.z ;
       printf("x = %.5f , z = %.5f \n",p.r.x,p.r.z);
        if(xz >= c_rad*c_rad){
            p.r = vec3(p.prev_r.x, p.prev_r.y ,(p.prev_r.z/p.prev_r.z)*sqrtf(c_rad*c_rad - p.prev_r.x*p.prev_r.x) ) ;
            vec3 r((p.r.x+p.prev_r.x)/2 , 0.0f , (p.r.z+p.prev_r.z)/2);
            vec3 u = p.r - p.prev_r  ;
            u.x = 0.0f ;
            vec3 v = u - 2*(r*u)*r.normalize() ;
            v.y = p.r.y ;
            p.r = v ;
        }*/
}
static void update(){
    int currTime=glutGet(GLUT_ELAPSED_TIME);
    dt=(currTime-lastTime);
    lastTime=currTime;
    dt=5;

    gridMap.clear();
    for(int i = 0; i < P.size();i++)
    {
        vec3 newGridPos = P[i].r/gridSize;
        newGridPos.x = (int)(newGridPos.x);
        newGridPos.y = (int)(newGridPos.y);
        newGridPos.z = (int)(newGridPos.z);
        //if (test%10) cout << newGridPos.x << " " << newGridPos.y << " " << newGridPos.z << endl;
        gridMap[newGridPos].push_back(i);
    }

    calculatePressure() ;
    for(int i=0;i<P.size();i++){
        vec3 total_a(0.0f,0.0f,0.0f) ;
        vec3 f_viscosity(0.0f,0.0f,0.0f);
        vec3 f_pressure(0.0f,0.0f,0.0f);
        vec3 f_tension(0.0f,0.0f,0.0f);
        vec3  f_tension_norm(0.0f,0.0f,0.0f);

        //P[i].r += P[i].v*dt/300 ;
        P[i].v += vec3(0.0f,GRAVITY,0.0f)*dt/1000;
        int co = 0 ;
        int gridX = P[i].gridPos.x;
        int gridY = P[i].gridPos.y;
        int gridZ = P[i].gridPos.z;
        for(int a = -1; a < 2; a++)
        {
            gridX = gridX + a;
            if (gridX < 0) continue;
            for (int b = -1; b < 2; b++)
            {
                gridY = gridY + b;
                if (gridY < 0) continue;
                for (int c = -1; c < 2; c++)
                {
                    gridZ = gridZ + c;
                    if (gridZ < 0) continue;
                    vec3 gridKey = vec3(gridX,gridY,gridZ);

                    // cout << gridKey.x << " " << gridKey.y << " " << gridKey.z << endl;

                    //SPH
                    //Smoothing Kernel Wpoly6
                    //Viscosity
                    for (vector<int>::iterator it = gridMap[gridKey].begin(); it != gridMap[gridKey].end(); it++)
                    {
                        vec3 rv = P[i].r-P[*it].r ;
                        float r = rv.length() ;
                        if(i == *it){}
                        else if ( 0 <= r && r <= H)
                        {
                            co++;
                            f_viscosity +=  P[*it].m*(P[*it].v-P[i].v)/P[*it].density * wGradient2ViscosityKernel(P[i].r ,P[*it].r).length() ;
                            f_pressure += P[*it].m*(P[*it].P+P[i].P)/(2*P[*it].density) * wGradientSpikyKernel(P[i].r ,P[*it].r) ;
                            //Calculate
                            f_tension += P[*it].m/P[*it].density * wGradient2SpikyKernel(P[i].r,P[*it].r);
                            f_tension_norm += P[*it].m/P[*it].density * wGradientSpikyKernel(P[i].r,P[*it].r) ;
                        }
                    }
                }
            }
        }

        // Calculate Total Force
        if (f_tension_norm.length() > SURFACE_THRESHOLD )f_tension = (-K_TENSION * f_tension ) * f_tension_norm / (f_tension_norm.length() +0.0001f) ;
        else f_tension = 0 ;
        f_viscosity = f_viscosity;
        f_pressure = -f_pressure ;
       // printf("Part %d Neigh= %d \n",i,co);
        total_a = (f_viscosity + f_tension +f_pressure  ) / P[i].density ;
        //printVec3(f_tension,"T");
       // printVec3(total_a,"Total");
        // Update Position and Velocity
       // P[i].v = P[i].v + total_a*dt/1000;
        //P[i].v.y += GRAVITY*dt/1000;

        P[i].prev_r = P[i].r ;
        P[i].r += P[i].v*dt/1000 +(total_a)*dt/1000*dt/1000;
        P[i].v =  (P[i].r - P[i].prev_r )/dt*1000;
        checkCollision(P[i]);
       // printVec3(P[i].v,"Ve ");
        //printf("Pos x = %.5f , y = %.5f ,z = %.5f \n",P[i].r.x,P[i].r.y,P[i].r.z);

    }

}

static void display(void)
{
    glTranslatef(0,0,-5.5f);
    glRotatef(angle,0,1,0);
    glTranslatef(0,0,5.5f);


    // Test Vertex Shader
    glPushMatrix();

	glPointSize(2.0f);
	glColor3f(0.9f, 0.9f, 1.0f);
	for(int i=0; i<10; i++)
	{
		glBegin(GL_POINTS);
			glVertex3f(-1+0.1f * i ,-1+0.1f * i ,0.1f * i );
		glEnd();
	}
    glPopMatrix();

    update();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
        for(int i=0;i<P.size();i++){
            P[i].draw();
        }
    plane.draw();


    glPopMatrix();
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    if(key==' '){

        float rx=x/(float)glutGet(GLUT_WINDOW_WIDTH)-0.5f;
        float ry=-1*(y/(float)glutGet(GLUT_WINDOW_HEIGHT)-0.5f);

        //cout<<rx<<':'<<ry<<endl;
        Particle part(vec3 (rx*5,ry*5,-5),1.0);
        part.v = vec3 (0,0,-2);
        vec3 newGridPos = part.r/gridSize;
        gridMap[newGridPos].push_back(++numBall);

        P.push_back(part);
    }
    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_DOWN)
	{
        buttonState = 1;
	}
    else if (state == GLUT_UP)
	{
        buttonState = 0;
        angle=0;
	}
	}
	xOrigin=x;

}
void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (buttonState==1) {

		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.5f;
        angle=deltaAngle;

		// update camera's direction
		lx = sin(deltaAngle);
		lz = -cos(deltaAngle);
	}
	xOrigin=x;

}

static void idle(void)
{
    glutPostRedisplay();
//    test++ ;
//    if (test % 100 ) {
//        for(map<vec3, vector<int>, CompareVectors>::iterator it1 = gridMap.begin(); it1 != gridMap.end(); it1++)
//        {
//            cout << it1->first.x << " " << it1->first.y << " " << it1->first.z << " has " << it1->second.size() << endl;
////            for (vector<int>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
////            {
////                cout << *it2 << " ";
////            }
////            cout << endl;
//        }
//        //while(1) {}
//    }
       /* test++;
        if(test > 4){
            while(1){}
        }*/
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

    glClearColor(0,0,0,0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glEnable( GL_BLEND );

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    //camera movement
    glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);


    init();
    glutMainLoop();

    return EXIT_SUCCESS;
}
