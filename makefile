all:
	gcc -g -Wall mainwnd.c -o mainwnd.so -fPIC -shared
	gcc -g -Wall calc.c -o calc.so -fPIC -shared
	gcc -g -Wall files.c -o files.so -fPIC -shared
	g++ -g -Wall -o twn twn.c window.c windowStore.cpp -lncursesw -DNCURSES_WIDECHAR=1
