#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

typedef struct THDL {
  char wndTitle[100];
  char className[100];

  int x;
  int y;
  int w;
  int h;
  int uid;

  struct THDL* parent;

  void (*thdlCallback)(struct THDL*, int);
  void (*thdlInputCallback)(struct THDL*, char*);

  int childClass;
  int winflags;

  int BUTTON_CLICKED_FRAMES=0;

  char hidden;

  bool checkboxChecked;
} THDL;

enum CALLBACK_MESSAGES {
  CALLBACK_CLICKED = 0x0001,
};

enum THDL_CHILDCLASSES {
  THDL_BUTTON   = 0x0001,
  THDL_INPUT    = 0x0002,
  THDL_LABEL    = 0x0003,
  THDL_CHECKBOX = 0x0004,
};

enum THDL_WINFLAGS {
  THDL_NORESIZE     = 0x0001,
};
