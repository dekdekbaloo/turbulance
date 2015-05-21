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
#include "include/Kernel.h"
#include <vector>
#include <stdlib.h>
#include <map>
#include <set>
#include <math.h>
#include <stdio.h>


// #define W_VISCOSITY_CONT 0.000894f
#define W_VISCOSITY_CONT 10000.0f
#define K_GAS 800.0f

#define K_TENSION 100.0f
#define WATER_DENSITY  800.0f
#define DENS 800.0f
#define PI 3.141592653589
#define REST_DENSITY 800.0f
#define SURFACE_THRESHOLD 1.00f
#define COLLIDE_CONSTANT 0.99f

static float gridSize = 0.2f;
static float GRAVITY = -9.8f;
static int numBall = 700;
static float startX = -0.5f;;
static float sizeX = 3.0f;
static float startY = -1.4f;
static float sizeY = 100; //Not yet used
static float startZ = -6.0f;
static float sizeZ = 2.0f;
static float c_rad = 4.0f;

struct coord {
    float x, y, z;

    bool operator==(const coord &o) const {
        return x == o.x && y == o.y && z == o.z;
    }

    bool operator<(const coord &o) const {
        return x < o.x || (x == o.x && y < o.y) || (x == o.x && y == o.y && z < o.z);
    }
    coord(float a, float b, float c) : x(a) , y(b) , z(c) {}
};

class Particle
{
    private:

    public:
        vec3 r;
        vec3 prev_r;
        vec3 v;
        float m;
        float density = WATER_DENSITY ;
        float P;
        coord gridPos;
        Particle(vec3 position,float mass) : r(position) , m(mass), gridPos(coord(0.0f,0.0f,0.f)) {}
        void draw(){
           glColor3d(0,0.5f,0.5f);

            glPushMatrix();
                glTranslated(r.x,r.y,r.z);
                glutSolidSphere(0.1f,10,10);
            glPopMatrix();

//            glPushMatrix();
//            glPointSize(1.0f);
//
//            glColor3f(v.x*2, v.y*2,v.z*5 );
//
//            glBegin(GL_POINTS);
//			glVertex3f(r.x,r.y,r.z);
//            glEnd();
             glPopMatrix();
        }
};

typedef map<coord, vector<int> > VectorMap;

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
        int kx = rand()%40000 ;
        int ky = rand()%20000 ;
        int kz = rand()%40000 ;
        //Particle part(vec3 (-1,5,1) ,1.0f);
        Particle part(vec3 (-0.5+0.00005*kx,0.1+0.0003*ky,-5+0.00001*kz),1.0f);

        //Particle part(vec3 (-2,0,-6),1) ;
        part.v.x = rand()%100/1000.0 ;
        part.v.y = rand()%100/1000.0 ;
        part.v.z = rand()%100/1000.0 ;
        part.v = vec3 (0,0,0);
        vec3 newGridPos = part.r/gridSize;
        newGridPos.x = (int)(newGridPos.x);
        newGridPos.y = (int)(newGridPos.y);
        newGridPos.z = (int)(newGridPos.z);
        coord co(newGridPos.x,newGridPos.y,newGridPos.z);
        part.gridPos = co;

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

static void checkCollision(Particle &p)
{
    //Box Collision checking
        if(p.r.y< startY){
            p.r.y = startY+0.001;
            //p.v.y = 0;
            p.v.y= -0.4f*p.v.y;

        }
        if(p.r.x< startX){
            p.r.x = startX+0.001 ;
            p.v.x=-COLLIDE_CONSTANT*p.v.x;
        }
        if(p.r.x> startX+sizeX){
            p.r.x = startX+sizeX-0.001 ;
            p.v.x=-COLLIDE_CONSTANT*p.v.x;
        }
        if(p.r.z< startZ){
            p.r.z = startZ+0.001;
            p.v.z=-COLLIDE_CONSTANT*p.v.z;
        }
        if(p.r.z> startZ+sizeZ){
            p.r.z = startZ+sizeZ-0.001;
            p.v.z=-COLLIDE_CONSTANT*p.v.z;
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

//    gridMap.clear();
//    for(int a = (int)(startX/gridSize); a < (int)((startX+sizeX)/gridSize); a++)
//    {
//        for(int b = (int)(startY/gridSize); b < (int)((startY+sizeY)/gridSize); b++)
//        {
//            for(int c = (int)(startZ/gridSize); c < (int)((startZ+sizeZ)/gridSize); c++)
//            {
//                coord v(a,b,c);
//                gridMap[v] = vector<int>();
//            }
//
//        }
//
//    }

//    for(int i = 0; i < P.size();i++)
//    {
//        vec3 newGridPos = P[i].r/gridSize;
//        newGridPos.x = (int)(newGridPos.x);
//        newGridPos.y = (int)(newGridPos.y);
//        newGridPos.z = (int)(newGridPos.z);
//        coord c(newGridPos.x,newGridPos.y,newGridPos.z);
//        P[i].gridPos = c;
//        //cout << newGridPos.x << " " << newGridPos.y << " " << newGridPos.z << endl;
//        gridMap[c].push_back(i);
//        //cout << gridMap[c].size() << endl;
//    }
//
//    //cout << endl << endl << endl;


    for(int i=0;i<P.size();i++){
        vec3 total_a(0.0f,0.0f,0.0f) ;
        vec3 f_viscosity(0.0f,0.0f,0.0f);
        vec3 f_pressure(0.0f,0.0f,0.0f);
        vec3 f_tension(0.0f,0.0f,0.0f);
        vec3  f_tension_norm(0.0f,0.0f,0.0f);

        //P[i].r += P[i].v*dt/300 ;
        P[i].v += vec3(0.0f,GRAVITY,0.0f)*dt/1000;
        int co = 0 ;
//        int gridX = P[i].gridPos.x;
//        int gridY = P[i].gridPos.y;
//        int gridZ = P[i].gridPos.z;

        float density = DENS ;
        float nearDensity = 0;

        for (int j = 0; j < P.size(); j++)
        {
            vec3 rv = P[i].r-P[j].r ;
            float r = rv.length() ;
            if(i != j && 0 <= r && r <= H)
            {
                co++ ;
                //cout << gridMap[gridKey].size() << " " << gridKey.x << " " << gridKey.y << " " << gridKey.z << "   " << *it << endl;
                Particle pj = P[j];
                vec3 dis = P[i].r - pj.r ;
                if (dis.length() > H*H) continue ;
                density += wPoly6Kernel(P[i].r , pj.r);
                //printf("%f\n",density);
                //co++;
                f_viscosity +=  P[j].m*(P[j].v-P[i].v)/P[j].density * wGradient2ViscosityKernel(P[i].r ,P[j].r) ;
                f_pressure += P[j].m*(P[j].P+P[i].P)/(2*P[j].density) * wGradientSpikyKernel(P[i].r ,P[j].r) ;
                //Calculate
                f_tension += P[j].m/P[j].density * wGradient2Poly6Kernel(P[i].r,P[j].r);
                f_tension_norm += P[j].m/P[j].density * wGradientPoly6Kernel(P[i].r,P[j].r) ;

            }
        }

        P[i].density = density;
        P[i].P = K_GAS * (density - REST_DENSITY);

        // Calculate Total Force
        if (f_tension_norm.length() > SURFACE_THRESHOLD )f_tension=(-K_TENSION * f_tension ) * f_tension_norm / (f_tension_norm.length() +0.0001f) ;

        else f_tension = 0 ;
        f_viscosity = W_VISCOSITY_CONT * f_viscosity;
        f_pressure = -f_pressure ;
        // printf("Part %d Neigh= %d \n",i,co);
        total_a = (f_viscosity + f_tension +f_pressure  ) / P[i].density ;
        //printVec3(f_tension,"T");
        //printVec3(f_viscosity,"V");
        //printVec3(total_a,"Total");
        //printf("\n");
        // Update Position and Velocity
        // P[i].v = P[i].v + total_a*dt/1000;
        //P[i].v.y += GRAVITY*dt/1000;
        P[i].prev_r = P[i].r ;
        P[i].r += P[i].v*dt/1000 +(total_a)*dt/1000*dt/1000*COLLIDE_CONSTANT;
        P[i].v =  (P[i].r - P[i].prev_r )/dt*1000*COLLIDE_CONSTANT;
        checkCollision(P[i]);
        // printVec3(P[i].v,"Ve ");
        // printf("Pos x = %.5f , y = %.5f ,z = %.5f \n",P[i].r.x,P[i].r.y,P[i].r.z);


    }

    //printf("\n\n");

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
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
	xOrigin=x;

}

static void idle(void)
{
    glutPostRedisplay();
    test++ ;
//    if (test>1) {
//        for(map<coord, vector<int> >::iterator it1 = gridMap.begin(); it1 != gridMap.end(); it1++)
//        {
//            cout << it1->first.x << " " << it1->first.y << " " << it1->first.z << " has " << it1->second.size() << endl;
////            for (vector<int>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
////            {
////                cout << *it2 << " ";
////            }
////            cout << endl;
//        }
//        cout << endl;
//        //while(1) {}
//    }
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
    //camera movement
    glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);


    init();
    glutMainLoop();

    return EXIT_SUCCESS;
}
