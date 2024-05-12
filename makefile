all:
	gcc testApi.c -ldl -shared -fPIC -o testApi.so
	g++ -g -o twn twn.c window.c windowStore.cpp -lncursesw
	cp ../twn /mnt/c/users/aceinet -r
