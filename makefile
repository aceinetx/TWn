all:
	gcc -g mainwnd.c -o mainwnd.so -fPIC -shared
	gcc -g calc.c -o calc.so -fPIC -shared
	gcc -g files.c -o files.so -fPIC -shared
	g++ -g -o twn twn.c window.c windowStore.cpp -lncursesw -DNCURSES_WIDECHAR=1
