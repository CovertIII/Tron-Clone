CFLAGS=-g
MACFLAGS=-framework GLUT -framework OpenGL 
LINUXFLAGS=-lGL -GLU -lglut
COCOA=-framework Cocoa
NETLIBS= /usr/local/lib/libenet.a /usr/local/lib/libtpl.a
AUDIO=-framework OpenAL /usr/local/lib/libvorbis.a /usr/local/lib/libogg.a /usr/local/lib/libvorbisfile.a
PROGRAMS=tron
GAMEOBJ=vector2.o particle_sys.o collison.o plist.o traillist.o player.o arena.o
NETWORK=user.o chat.o
AUDIOOBJ = load_sound.o sound_list.o

CC=gcc

all: tron tserver tclient

tserver: main_server.o server.o $(NETWORK) $(GAMEOBJ)
	$(CC) $(CFLAGS) $(AUDIO) $(AUDIOOBJ) $(NETLIBS) server.o $(MACFLAGS) main_server.o $(NETWORK) $(GAMEOBJ) -o tserver

main_server.o: main_server.c server.h
	$(CC) $(CFLAGS) -c main_server.c

tclient: main_client.o client.o $(NETWORK) $(GAMEOBJ) $(AUDIOOBJ)
	$(CC) $(CFLAGS) $(AUDIO) $(AUDIOOBJ) $(NETLIBS) client.o $(MACFLAGS) main_client.o $(NETWORK) $(GAMEOBJ) -o tclient 

main_client.o: main_client.c client.h vector2.h
	$(CC) $(CFLAGS) -c main_client.c

tron: main.o $(GAMEOBJ)
	$(CC) $(CFLAGS) $(AUDIO) $(AUDIOOBJ) $(NETLIBS) $(MACFLAGS) $(COCOA) main.o $(GAMEOBJ) -o tron

main.o:	main.c arena.h
	$(CC) $(CFLAGS) -c main.c

vector2.o: vector2.c vector2.h
	$(CC) $(CFLAGS) -c vector2.c

particle_sys.o: particle_sys.c particle_sys.h vector2.h
	$(CC) $(CFLAGS) -c particle_sys.c

collison.o: collison.c collison.h vector2.h
	$(CC) $(CFLAGS) -c collison.c

plist.o: plist.c plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c plist.c

traillist.o: traillist.c traillist.h plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c traillist.c

player.o: player.c player.h traillist.h particle_sys.h plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c player.c

arena.o: arena.c arena.h traillist.h plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c arena.c

chat.o: chat.c chat.h
	$(CC) $(CFLAGS) -c chat.c

user.o: user.c user.h
	$(CC) $(CFLAGS) -c user.c

server.o: server.c server.h user.h chat.h arena.h
	$(CC) $(CFLAGS) -c server.c

load_sound.o: load_sound.c load_sound.h
	$(CC) $(CFLAGS) -c load_sound.c

sound_list.o: sound_list.c sound_list.h vector2.h
	$(CC) $(CFLAGS) -c sound_list.c

client.o: client.c client.h user.h vector2.h load_sound.h sound_list.h chat.h arena.h
	$(CC) $(CFLAGS) -c client.c

clean:
	\rm *.o
	
