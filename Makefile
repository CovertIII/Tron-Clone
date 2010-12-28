CFLAGS=-g
PROGRAMS=tron
LIBS=vector2.o collison.o plist.o traillist.o player.o arena.o

CC=gcc

tron: main.o $(LIBS)
	$(CC) $(CFLAGS) -framework GLUT -framework OpenGL -framework Cocoa main.o $(LIBS) -o tron

main.o:	main.c arena.h
	$(CC) $(CFLAGS) -c main.c

vector2.o: vector2.c vector2.h
	$(CC) $(CFLAGS) -c vector2.c

collison.o: collison.c collison.h vector2.h
	$(CC) $(CFLAGS) -c collison.c

plist.o: plist.c plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c plist.c

traillist.o: traillist.c traillist.h plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c traillist.c

player.o: player.c player.h traillist.h plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c player.c

arena.o: arena.c arena.h traillist.h plist.h collison.h vector2.h
	$(CC) $(CFLAGS) -c arena.c

clean:
	\rm *.o