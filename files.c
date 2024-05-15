#include "./apiSymbolsStore.h"
#include "./window.h"
#include <string.h>

#define FILES_LENGTH 30

THDL labelPath;

void* twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;  // where we will store api functions
  symbols = *(struct apiSymbolsStore*)sym;

  THDL window;
  strcpy(window.wndTitle, "Files");
  strcpy(window.className, "files");
  window.x = 10;
  window.y = 10;
  window.w = 70;
  window.h = 15;
  window.winflags = THDL_NORESIZE;
  symbols.appendWindow(&window);

  strcpy(labelPath.wndTitle, "/");
  strcpy(labelPath.className, "labelpath");
  labelPath.x = 1;
  labelPath.y = 2;
  labelPath.w = 1; labelPath.h = 1;
  labelPath.childClass = THDL_LABEL;
  labelPath.parent = &window;
  symbols.appendWindow(&labelPath);

  THDL files[FILES_LENGTH];
  for(int i=0; i<FILES_LENGTH; i++){
    THDL *wnd = &files[i];
    wnd->x = 1;
    wnd->y = 4;
    wnd->w = 1; wnd->h = 1;
    wnd->childClass = THDL_LABEL;
    wnd->parent = &window;
    symbols.appendWindow(&files[i]);
  }

  while(1){}
} 
