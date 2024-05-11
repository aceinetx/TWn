#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include "./window.h"
#include "./apiSymbolsStore.h"
#include <string.h>

void twnEntry(
  struct apiSymbolsStore symbols
)
{
  THDL secondW;
  strcpy(secondW.wndTitle, "Api window");
  strcpy(secondW.className, "Api window");
  secondW.x = 50;
  secondW.y = 40;
  secondW.w = 10;
  secondW.h = 10;

  //symbols.appendWindow(&secondW);
  for(int i=0; i<symbols.getWindowStoreLength(); i++){
    THDL* wnd = symbols.getWindowByIndex(i);
    if(wnd->wndTitle[0] == 'T'){
      strcpy(wnd->wndTitle, "Detected");
    }
  }
}

