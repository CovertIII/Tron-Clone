#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include "arena.h"
#include "vector2.h"

#define TRUE 1
#define FALSE 0
#define TIMERMSECS 1

int lastFrameTime = 0;
int frame=0,time, fps, timebase=0;

vector2 movin = {0,0};
vector2 mouse = {0,0};

arena mygame;

void circle_object(float pos_x, float pos_y, float size) {
	glPushMatrix();
	glTranslatef(pos_x,pos_y,0);
	float counter;
	glBegin(GL_POLYGON);
	for (counter = 0; counter <= 2*3.14159; counter = counter + 3.14159/12) {
		glVertex3f ((size)*cos(counter), (size)*sin(counter), 0.0);
	}
    glEnd();
	glPopMatrix();
}	


void renderBitmapString(
						float x, 
						float y, 
						void *font,
						char *string) {  
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void init()
{
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	mygame = arena_init(10,0, windowWidth, windowHeight);
}

void numbers(int value){
	glutTimerFunc(TIMERMSECS, numbers, 0);
	int i,j,k;
	
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	
	int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;
	float h = elapsedTime;
	
	arena_update(mygame, h);
	glutPostRedisplay();
}


void display(void) {
	
	int i,j;
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	//numbers();
	
	//-----This is the stuff involved with drawing the screen----//	
	glClearColor (0.8, 0.8,0.8,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	
	if (time - timebase > 500) {
		fps = frame*1000.0/(time-timebase);
	 	timebase = time;		
		frame = 0;
	}
	
	char buf[30];
	sprintf(buf, "FPS: %i", fps);	
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1, 1, 1);
	renderBitmapString(10, windowHeight-10, GLUT_BITMAP_TIMES_ROMAN_10, buf);
	glPopMatrix();
	
	arena_render(mygame);
	
	glutSwapBuffers();
	glFlush();
}

void pressKey(unsigned char key, int xx, int yy) {
	switch(key) {
		case '1':
			
			break;
		case ' ':
			
			break;
		case 27 :
			exit(0);
			break;
		default:
			break;
	}
}

void SpressKey(int key, int xx, int yy) {
	switch(key) {
		case GLUT_KEY_LEFT:
			arena_ply_turn(mygame, 0, -3);
			break;
		case GLUT_KEY_RIGHT:
			arena_ply_turn(mygame, 0,  3);
			break;
		case GLUT_KEY_UP:
			movin.y=1;
			break;
		case GLUT_KEY_DOWN:
			movin.y=-1;
			break;
	}
	glutPostRedisplay();
}

void releaseKey(unsigned char key, int xx, int yy) {
	switch (key) {
		default:
			break;
	}
}

void SreleaseKey(int key, int xx, int yy) {
	switch(key) {
		case GLUT_KEY_LEFT:
			arena_ply_turn(mygame, 0, 0);
			break;
		case GLUT_KEY_RIGHT:
			arena_ply_turn(mygame, 0, 0);
			break;
		case GLUT_KEY_UP:
			if(movin.y>0)
			movin.y=0;
			break;
		case GLUT_KEY_DOWN:
			if(movin.y<0)
			movin.y=0;
			break;
	}
}

void mouseButton(int bbutton, int sstate, int xx, int yy)
{
	yy=glutGet(GLUT_WINDOW_HEIGHT)-yy;	
	mouse.x = xx;
	mouse.y = yy;
	
}

void mouseMotion(int xx, int yy) 
{
	yy=glutGet(GLUT_WINDOW_HEIGHT)-yy;	
	mouse.x = xx;
	mouse.y = yy;
}


void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("TronClone");
	
	init();
	
	
	glutMouseFunc(mouseButton);
	glutPassiveMotionFunc(mouseMotion);
	glutMotionFunc(mouseMotion);
	glutIgnoreKeyRepeat(1);   
	glutSpecialFunc(SpressKey);
	glutSpecialUpFunc(SreleaseKey);
	glutKeyboardFunc(pressKey);
	glutKeyboardUpFunc(releaseKey);
	glutDisplayFunc(display);
  	glutReshapeFunc(reshape);
	glutPostRedisplay();
	
	glutTimerFunc(TIMERMSECS, numbers, 0);
	
    glutMainLoop();
    return EXIT_SUCCESS;
	
}