all:
	gcc -framework GLUT -framework OpenGL -framework Cocoa collison.c main.c traillist.c vector2.c player.c arena.c plist.c -o tron
