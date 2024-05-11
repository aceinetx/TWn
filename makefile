all:
	g++ -o twn twn.c window.c windowStore.cpp -lncurses
	cp ../twn /mnt/c/users/aceinet -r
