#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include "./window.h"
#include "./apiSymbolsStore.h"
#include <string.h>

void *twnEntry(
  void *sym
)
{
  struct apiSymbolsStore symbols;
  symbols = *(struct apiSymbolsStore*)sym;
  THDL secondW;
  strcpy(secondW.wndTitle, "Api window");
  strcpy(secondW.className, "Api window");
  secondW.x = 50;
  secondW.y = 30;
  secondW.w = 10;
  secondW.h = 10;
  symbols.appendWindow(&secondW);
  while(1){};
}
