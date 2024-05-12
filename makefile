all:
	gcc -o calc.so calc.c -shared -fPIC
	g++ -g -o twn twn.c window.c windowStore.cpp -lncursesw
