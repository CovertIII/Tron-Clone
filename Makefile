all:
	gcc -framework GLUT -framework OpenGL -framework Cocoa main.c vector2.c player.c arena.c plist.c -o tron