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
#include <vector>
#include <stdlib.h>
#include <map>
#include <set>
#include <math.h>
#include <stdio.h>

// #define W_VISCOSITY_CONT 0.000894f
#define W_VISCOSITY_CONT 4.80f
#define K_GAS 0.082057f
#define K_TENSION 0.04f
#define GLEW_STATIC 50

static float gridSize = 0.2f;
static float GRAVITY = -2.5f ;
static int numBall = 300;
static float startX = -0.5f;;
static float sizeX = 1.0f;
static float startY = -1.4f;
static float sizeY = 100; //Not yet used
static float startZ = -6.0f;
static float sizeZ = 1.0f;

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

GLuint v;
GLuint f;
GLuint p;

int lastTime=0;
static float dt;
static vector<Particle> P;
Plane plane;
static VectorMap gridMap;

/*void set_shaders()
{
	char *vs=NULL;
	char *fs=NULL;

	vs=(char *)malloc(sizeof(char)*10000);
	fs=(char *)malloc(sizeof(char)*10000);
	memset(vs, 0, sizeof(char)*10000);
	memset(fs, 0, sizeof(char)*10000);

	FILE *fp;
	char c;
	int count;

	fp=fopen("Shader/shader.vs", "r");
	count=0;
	while((c=fgetc(fp)) != EOF)
	{
		vs[count]=c;
		count++;
	}
	fclose(fp);

	fp=fopen("Shader/shader.fs", "r");
	count=0;
	while((c=fgetc(fp)) != EOF)
	{
		fs[count]=c;
		count++;
	}
	fclose(fp);

	v=glCreateShader(GL_VERTEX_SHADER);
	f=glCreateShader(GL_FRAGMENT_SHADER);

	const char *vv;
	const char *ff;
	vv=vs;
	ff=fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	int success;

	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char info_log[5000];
		glGetShaderInfoLog(v, 5000, NULL, info_log);
		printf("Error in vertex shader compilation!\n");
		printf("Info Log: %s\n", info_log);
	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char info_log[5000];
		glGetShaderInfoLog(f, 5000, NULL, info_log);
		printf("Error in fragment shader compilation!\n");
		printf("Info Log: %s\n", info_log);
	}

	p=glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);
	glUseProgram(p);

	free(vs);
	free(fs);
}*/
/*
static void set_shaders(){
    std::string vertexSource = "Shader/shader.vs" ;
    std::string fragmentSource = "Shader/shader.fs" ;
    v = glCreateShader(GL_VERTEX_SHADER);

    //Send the vertex shader source code to GL
    //Note that std::string's .c_str is NULL character terminated.
    const GLchar *source = (const GLchar *)vertexSource.c_str();
    glShaderSource(v, 1, &source, 0);

    //Compile the vertex shader
    glCompileShader(v);

    GLint isCompiled = 0;
    glGetShaderiv(v, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(v, maxLength, &maxLength, &infoLog[0]);

        //We don't need the shader anymore.
        glDeleteShader(v);

        //Use the infoLog as you see fit.
        //In this simple program, we'll just leave
        return;
    }
    f = glCreateShader(GL_FRAGMENT_SHADER);

    //Send the fragment shader source code to GL
    //Note that std::string's .c_str is NULL character terminated.
    source = (const GLchar *)fragmentSource.c_str();
    glShaderSource(f, 1, &source, 0);

    //Compile the fragment shader
    glCompileShader(f);

    glGetShaderiv(f, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(f, maxLength, &maxLength, &infoLog[0]);

        //We don't need the shader anymore.
        glDeleteShader(f);
        //Either of them. Don't leak shaders.
        glDeleteShader(v);

        //Use the infoLog as you see fit.

        //In this simple program, we'll just leave
        return;
    }else{
        printf("Ready!");
    }

    //Vertex and fragment shaders are successfully compiled.
    //Now time to link them together into a program.
    //Get a program object.
    p = glCreateProgram();

    //Attach our shaders to our program
    glAttachShader(p, v);
    glAttachShader(p, f);

    //Link our program
    glLinkProgram(p);

    //Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(p, GL_LINK_STATUS, (int *)&isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(p, maxLength, &maxLength, &infoLog[0]);

        //We don't need the program anymore.
        glDeleteProgram(p);
        //Don't leak shaders either.
        glDeleteShader(v);
        glDeleteShader(f);

        //Use the infoLog as you see fit.

        //In this simple program, we'll just leave
        return;
    }

    //Always detach shaders after a successful link.
    glDetachShader(p, v);
    glDetachShader(p, f);
}*/
static void printVec3(vec3 a , const char *string)
{
    printf (string);
   printf(" : %f %f %f \n",a.x,a.y,a.z);
}
static void init(){
    //Create grid
    for (int i = 0; i < (int)(sizeX/gridSize); i++)
    {
        for (int j = 0; j < (int)(sizeY/gridSize); j++)
        {
            for (int k = 0; k < (int)(sizeZ/gridSize); k++)
            {
                gridMap[vec3(i,j,k)] = vector<int>();
            }
        }
    }
    // Create Particle
    for(int i=0;i<numBall;i++){
        int kx = rand()%500 ;
        int ky = rand()%200 ;
        int kz = rand()%200 ;
        Particle part(vec3 (0.5+0.003*kx,0.1+0.02*ky,6-0.01*kz),1.0);

        //Particle part(vec3 (0.5,0.1,6),1.0);

        //Particle part(vec3 (-2,0,-6),1) ;
        //part.v.z = rand()%100/100000.0 ;
        part.v = vec3 (0,0,0);
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

static void update(){
    int currTime=glutGet(GLUT_ELAPSED_TIME);
    dt=(currTime-lastTime);
    lastTime=currTime;

    for(map<vec3, vector<int>, CompareVectors>::iterator it1 = gridMap.begin(); it1 != gridMap.end(); it1++)
    {
        it1->second.clear();
    }

    for(int i = 0; i < P.size();i++)
    {
        vec3 newGridPos = P[i].r/gridSize;
        gridMap[newGridPos].push_back(i);
    }

    for(int i=0;i<P.size();i++){
        vec3 total_a(0.0f,0.0f,0.0f) ;
        vec3 f_viscosity(0.0f,0.0f,0.0f);
        vec3 f_pressure(0.0f,0.0f,0.0f);
        vec3 f_tension(0.0f,0.0f,0.0f);

        int gridX = P[i].gridPos.x;
        int gridY = P[i].gridPos.y;
        int gridZ = P[i].gridPos.z;
        for(int a = -1; a < 2; a=a+2)
        {
            gridX = gridX + a;
            if (gridX < 0) continue;
            for (int b = -1; b < 2; b=b+2)
            {
                gridY = gridY + b;
                if (gridY < 0) continue;
                for (int c = -1; c < 2; c=c+2)
                {
                    gridZ = gridZ + c;
                    if (gridZ < 0) continue;
                    vec3 gridKey = vec3(gridX,gridY,gridZ);

                    //cout << gridKey.x << " " << gridKey.y << " " << gridKey.z << endl;

                    //SPH
                    //Smoothing Kernel Wpoly6
                    //Viscosity
                    for (vector<int>::iterator it = gridMap[gridKey].begin(); it != gridMap[gridKey].end(); it++)
                    {
                        if(i == *it){}
                        else
                        {
                            f_viscosity +=  P[*it].m*(P[*it].v-P[i].v)/P[*it].density * wGradient2ViscosityKernel(P[i].r ,P[*it].r) ;
                            f_pressure += P[*it].m*(2*K_GAS*P[*it].density)/(2*P[*it].density) * wGradientSpikyKernel(P[i].r ,P[*it].r) ;
                            f_tension += P[*it].m * wGradientSpikyKernel(P[i].r,P[*it].r).length()*(P[i].r -P[*it].r);
                        }
                    }
                }
            }
        }

        f_tension = f_tension * (K_TENSION /P[i].m);
        f_viscosity = -1*f_viscosity;

        total_a = (f_viscosity + f_pressure + f_tension  ) / WATER_DENSITY ;
       // printVec3(f_tension,"Tension");
      //  printf("A : %f %f %f \n",total_a.x,total_a.y,total_a.z);

        // printf("index : %d acc = %.12f\n",i,f_viscosity.length());
        // printf("Acc x = %.5f , y = %.5f ,z = %.5f \n",total_a.x,total_a.y,total_a.z);
        P[i].v = P[i].v + total_a*dt/1000;
        P[i].v.y += GRAVITY*dt/1000;
        P[i].r+= P[i].v*dt/1000;

        //Collision checking
        if(P[i].r.y<= startY){
            P[i].r.y= startY;
            //P[i].v.y = 0;
            P[i].v.y= -0.2f*P[i].v.y;
        }
        if(P[i].r.x<= startX){
            P[i].r.x= startX;
            P[i].v.x=-0.9f*P[i].v.x;
        }
        if(P[i].r.x>= startX+sizeX){
            P[i].r.x= startX+sizeX;
            P[i].v.x=-0.9f*P[i].v.x;
        }
        if(P[i].r.z<= startZ){
            P[i].r.z= startZ;
            P[i].v.z=-0.9f*P[i].v.z;
        }
        if(P[i].r.z>= startZ+sizeZ){
            P[i].r.z= startZ+sizeZ;
            P[i].v.z=-0.9f*P[i].v.z;
        }
        //printf("Pos x = %.5f , y = %.5f ,z = %.5f ",P[i].r.x,P[i].r.y,P[i].r.z);
    }

    //for (int i = 0; i < )
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
int c = 0 ;
static void idle(void)
{
    glutPostRedisplay();
    //c++ ;
    if (c == 1 ) {
        for(map<vec3, vector<int>, CompareVectors>::iterator it1 = gridMap.begin(); it1 != gridMap.end(); it1++)
        {
            cout << it1->first.x << " " << it1->first.y << " " << it1->first.z << endl;
            for (vector<int>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
            {
                cout << *it2 << " ";
            }
            cout << endl;
        }
        while(1) {}
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
    //camera movement
    glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);


    init();
    glutMainLoop();

    return EXIT_SUCCESS;
}
