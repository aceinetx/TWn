#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

typedef struct THDL {
  char wndTitle[25];
  char className[25];

  int x;
  int y;
  int w;
  int h;

  struct THDL* parent;

  void (*thdlCallback)(struct THDL*, int);
} THDL;
