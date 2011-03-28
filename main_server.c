
#include <signal.h>
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
void leave(int sig);

void clean_up(void){
	printf("Cleaning up resources used by tserver\n");
	server_free(tronserver);
	enet_host_destroy(enetserver);
}

int main (int argc, char ** argv){
	prev = getthetime();
	(void) signal(SIGINT,leave);

	if (enet_initialize () != 0){
      fprintf (stderr, "An error occurred while initializing ENet.\n");
      return EXIT_FAILURE;
	}

	atexit (enet_deinitialize);
		

	address.host = ENET_HOST_ANY;
	address.port = 5001;
	
	enetserver = enet_host_create (& address /* the address to bind the server host to */, 
							   32      /* allow up to 32 clients and/or outgoing connections */,
							   0      /* allow up to 2 channels to be used, 0 and 1 */,
							   0      /* assume any amount of incoming bandwidth */,
							   0      /* assume any amount of outgoing bandwidth */);
	if (enetserver== NULL){
		fprintf (stderr, 
                 "An error occurred while trying to create an ENet server host.\n");
		exit (EXIT_FAILURE);
	}

	tronserver = server_init(enetserver);
	atexit(clean_up);
	printf("This is the TronClone Server.\nPush ^c to kill the server.\n");

    /* Main game loop */
	while(1){
		int i, index, temp;
		char buf[80];
		if (enet_host_service (enetserver, & event, 10) > 0){
			switch (event.type){
    		    case ENET_EVENT_TYPE_CONNECT:
					enet_address_get_host_ip(&event.peer->address, buf, sizeof buf);
    		    	printf ("A new client connected from %s\n", 
					        buf);
					server_add_user(tronserver, event.peer);
    		      break;
    		
    		    case ENET_EVENT_TYPE_RECEIVE:
					server_process_packet(tronserver, event);
					enet_packet_destroy (event.packet);
    		    	break;
    		
    		    case ENET_EVENT_TYPE_DISCONNECT:		
					enet_address_get_host_ip(&event.peer->address, buf, sizeof buf);
					printf ("A client disconnected from %s: %u.\n", 
					        buf, 
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
	clean_up(); 
}

unsigned long int getthetime()
{
	struct timeval tv;

  	gettimeofday(&tv, NULL); 
 	
	return tv.tv_sec*1000000+tv.tv_usec;
}

void leave(int sig) {
	printf("\nProgram interupted.  Cleaning up.\n");
    exit(sig);
}
