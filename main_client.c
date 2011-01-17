#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLUT/glut.h>
#include <enet/enet.h>
#include "client.h"
#include "vector2.h"

vector2 movin = {0,0};

int lastFrameTime = 0;

ENetHost * enet_client;
ENetAddress address;
ENetEvent event;
client tclient;

int init_network(void){
	if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
	atexit (enet_deinitialize);

	enet_client = enet_host_create (NULL /* create a client host */,
							   1 /* only allow 1 outgoing connection */,
							   10/* allow up 4 channels to be used, 0 and 1 */,
							   57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
							   14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
   
   if (enet_client == NULL)
   {
       fprintf (stderr, 
                "An error occurred while trying to create an ENet client host.\n");
       exit (EXIT_FAILURE);
   }
   tclient = client_init(enet_client);
}


void processNormalKeys(unsigned char key, int xx, int yy) {
	client_keys(tclient, key);
}


void numbers(void){
	int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;
	float h = elapsedTime;
	
	client_update(tclient, h);

	while (enet_host_service (enet_client, & event, 10) > 0 && client_connection(tclient))
	{
	    switch (event.type)
	    {
	    	case ENET_EVENT_TYPE_RECEIVE:
				client_process_packets(tclient, &event);
	    		break;
    
	    	case ENET_EVENT_TYPE_DISCONNECT:
				client_disconnect(tclient);
				break;
	    }
	}
}

void display(void) {
	int i,k;
	numbers();
	//-----This is the stuff involved with drawing the screen----//	
	glClearColor (0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	client_render(tclient);	
	
	glutSwapBuffers();
	
}

void pressKey(int key, int xx, int yy) {
	switch(key) {
		case GLUT_KEY_LEFT : 
			movin.x=-1;
			break;
		case GLUT_KEY_RIGHT : 
			movin.x= 1;
			break;
		case GLUT_KEY_UP : 
			movin.y= 1;
			break;
		case GLUT_KEY_DOWN : 
			movin.y=-1;
			break;
	}
}

void releaseKey(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_LEFT :
			if(movin.x<0){movin.x=0;}
			break;
		case GLUT_KEY_RIGHT : 
			if(movin.x>0){movin.x=0;}
			break;
		case GLUT_KEY_UP :
			if(movin.y>0){movin.y=0;}
			break;
		case GLUT_KEY_DOWN :
			if(movin.y<0){movin.y=0;}
			break;
		default:
			break;
	}
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
  
  glutCreateWindow("Tron Clone Client");
	
	init_network();
	
	glutIgnoreKeyRepeat(1);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey); 
	
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
	glutKeyboardFunc(processNormalKeys);
    glutMainLoop();
	client_free(tclient);
   	enet_host_destroy(enet_client);
    return EXIT_SUCCESS;
	
}

