#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <enet/enet.h>
#include "server.h"

ENetAddress address;
ENetEvent event;
ENetHost * enetserver;

unsigned long int now;
unsigned long int prev;

server tronserver;

unsigned long int getthetime();

int main (int argc, char ** argv){
	prev = getthetime();

	if (enet_initialize () != 0){
      fprintf (stderr, "An error occurred while initializing ENet.\n");
      return EXIT_FAILURE;
	}

	atexit (enet_deinitialize);
	
	tronserver = server_init();

	address.host = ENET_HOST_ANY;
	address.port = 5001;
	
	enetserver = enet_host_create (& address /* the address to bind the server host to */, 
							   32      /* allow up to 32 clients and/or outgoing connections */,
							   2      /* allow up to 2 channels to be used, 0 and 1 */,
							   0      /* assume any amount of incoming bandwidth */,
							   0      /* assume any amount of outgoing bandwidth */);
	if (enetserver== NULL){
		fprintf (stderr, 
                 "An error occurred while trying to create an ENet server host.\n");
		exit (EXIT_FAILURE);
	}
	
	printf("This is the TronClone Server.\nPush ^c to kill the server.\n");

    /* Main game loop */
	while(1){
		int i, index, temp;
		if (enet_host_service (enetserver, & event, 10) > 0){
			switch (event.type){
    		    case ENET_EVENT_TYPE_CONNECT:
    		    	printf ("A new client connected from %x: %u.\n", 
					        event.peer -> address.host, 
					        event.peer -> address.port);
					server_add_user(tronserver, event.peer);
    		      break;
    		
    		    case ENET_EVENT_TYPE_RECEIVE:
					//server_process_packet(tronserver, event.packet->data);
					enet_packet_destroy (event.packet);
    		    	break;
    		
    		    case ENET_EVENT_TYPE_DISCONNECT:		
					printf ("A client disconnected from %x: %u.\n", 
					        event.peer -> address.host, 
					        event.peer -> address.port);
					
					server_remove_user(tronserver, event.peer);	
    		  		event.peer -> data = NULL;
					break;
    		}
		}
		
		now = getthetime();
		double h = (now-prev)/1000000.0f;
		server_update(tronserver, h);
		prev = now;
	}
 
	enet_host_destroy(enetserver);
}

unsigned long int getthetime()
{
	struct timeval tv;

  	gettimeofday(&tv, NULL); 
 	
	return tv.tv_sec*1000000+tv.tv_usec;
}

