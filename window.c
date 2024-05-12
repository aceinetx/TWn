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
  void (*thdlInputCallback)(struct THDL*, char*);

  int childClass;
} THDL;

enum CALLBACK_MESSAGES {
  CALLBACK_CLICKED = 0x0001,
};

enum THDL_CHILDCLASSES {
  THDL_BUTTON = 0x0001,
  THDL_INPUT  = 0x0002,
};
