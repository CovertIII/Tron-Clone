#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <enet/enet.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <GL/glut.h>
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "vector2.h"
#include "arena.h"

#define TRUE 1
#define FALSE 0
#define GAME 0
#define LOBBY 1
#define TIMERMSECS 17
#define TWIXT_TIME 5.0f

int lastFrameTime = 0;
int frame=0,trontime, fps, timebase=0;

vector2 movin = {0,0};
vector2 movin2 = {0,0};
vector2 mouse = {0,0};
int sharp=0;
int sharp2=0;
float timer = 0;
int game_state;
int player_num;
int xbd = 800;
int ybd = 600;

arena mygame;

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

void init(int argc, char** argv)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	if(argc < 2)
		{player_num = 2;}
	else
		{sscanf(argv[1],"%d",&player_num);}
	game_state = GAME;	
	mygame = arena_init(player_num,0, xbd, ybd);
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
	if (movin.x == -1)
		{sharp ? arena_ply_turn(mygame, 0, 7) : arena_ply_turn(mygame, 0, 3);}
			else if (movin.x == 1)
				{sharp ? arena_ply_turn(mygame, 0, -7) : arena_ply_turn(mygame, 0, -3);}
			else if (movin.x == 0)
				{ arena_ply_turn(mygame, 0, 0);}
			if (movin.y == -1)
				{arena_ply_speed(mygame, 0, -500);}
			else if (movin.y == 1)
				{arena_ply_speed(mygame, 0,  500);}
			else if (movin.y == 0)
				{arena_ply_speed(mygame, 0, 0);}
			
			if (movin2.x == -1)
				{sharp2 ? arena_ply_turn(mygame, 1, 7) : arena_ply_turn(mygame, 1, 3);}
			else if (movin2.x == 1)
				{sharp2 ? arena_ply_turn(mygame, 1, -7) : arena_ply_turn(mygame, 1, -3);}
			else if (movin2.x == 0)
				{ arena_ply_turn(mygame, 1, 0);}
			if (movin2.y == -1)
				{arena_ply_speed(mygame, 1, -500);}
			else if (movin2.y == 1)
				{arena_ply_speed(mygame, 1,  500);}
			else if (movin2.y == 0)
				{arena_ply_speed(mygame, 1, 0);}
			
			arena_update(mygame, h);
	switch(game_state){
		case GAME:	
			
			if(arena_player_status(mygame) <= 1){
				game_state = LOBBY;
				timer = 4.99;
			}
			break;
		case LOBBY:
			timer -= h;
			if(timer <= 0){
				arena_free(mygame);
				mygame = NULL;

				int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
				int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
				mygame = arena_init(player_num,0, xbd, ybd);

				game_state = GAME;
			}
			break;
	}
	glutPostRedisplay();
}


void display(void) {
	
	int i,j;
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	
	//-----This is the stuff involved with drawing the screen----//	
	glClearColor (0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	frame++;
	trontime=glutGet(GLUT_ELAPSED_TIME);
	
	if (trontime - timebase > 500) {
		fps = frame*1000.0/(trontime-timebase);
	 	timebase = trontime;		
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

	if(game_state == LOBBY){
		sprintf(buf, "%.0f", timer+0.5);	
		glPushMatrix();
		glLoadIdentity();
		glColor3f(0.5, 0.5, 0.5);
		renderBitmapString(windowWidth/2, windowHeight/2, GLUT_BITMAP_TIMES_ROMAN_24, buf);
		glPopMatrix();
	}

		
	glutSwapBuffers();
	glFlush();
}

void pressKey(unsigned char key, int xx, int yy) {
	switch(key) {
		case ',':
			sharp = 1;
			break;
		case ' ':
			sharp2 = 1;
			break;	
		case 'm':
			arena_plyr_tg(mygame, 0);
			break;
		case 'q':
			arena_plyr_tg(mygame, 1);
			break;
		case 'w':
			movin2.y=1;
			break;
		case 's':
			movin2.y=-1;
			break;
		case 'a':
			movin2.x=-1;
			break;
		case 'd':
			movin2.x= 1;
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
			movin.x=-1;
			break;
		case GLUT_KEY_RIGHT:
			movin.x=1;
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
		case ',':
			sharp = 0;
			break;
		case ' ':
			sharp2 = 0;
			break;
		case 'a':
			if(movin2.x<0)
			movin2.x=0;
			break;
		case 'd':
			if(movin2.x>0)
			movin2.x=0;
			break;
		case 'w':
			if(movin2.y>0)
			movin2.y=0;
			break;
		case 's':
			if(movin2.y<0)
			movin2.y=0;
			break;
		default:
			break;
	}
}

void SreleaseKey(int key, int xx, int yy) {
	switch(key) {
		case GLUT_KEY_LEFT:
			if(movin.x<0)
			movin.x=0;
			break;
		case GLUT_KEY_RIGHT:
			if(movin.x>0)
			movin.x=0;
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
	
	double ratio = glutGet(GLUT_WINDOW_WIDTH)/(double)glutGet(GLUT_WINDOW_HEIGHT);
	vector2 w;
	w.x = xbd*1.2;
	w.y = ybd*1.2;

	double wx = ratio * w.y;
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, wx, 0, w.y);
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
    
    glutCreateWindow("Trail Blazer");
	
	init(argc, argv);
	
	
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
