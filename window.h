#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

struct THDL {
  char wndTitle[25];
  char className[25];

  int x;
  int y;
  int w;
  int h;

  THDL* parent = NULL;

  void (*thdlCallback)(THDL*, int)=NULL;
};

