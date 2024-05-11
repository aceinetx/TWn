all:
	gcc testApi.c -ldl -shared -fPIC -o testApi.so
	g++ -o twn twn.c window.c windowStore.cpp -lncurses
	cp ../twn /mnt/c/users/aceinet -r
