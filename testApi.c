#include "./window.h"
#include "./apiSymbolsStore.h"
#include <string.h>

THDL label;

void onInput(THDL* sender, char* text){
  snprintf(label.wndTitle, 50, "Text typed: %s", text);
}

void *twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;
  symbols = *(struct apiSymbolsStore*)sym;

  THDL secondW;
  strcpy(secondW.wndTitle, "Api window");
  strcpy(secondW.className, "Api window");
  secondW.x = 10;
  secondW.y = 10;
  secondW.w = 50;
  secondW.h = 20;

  strcpy(label.wndTitle, "Text typed: ");
  strcpy(label.className, "typed");
  label.x = 1;
  label.y = 2;
  label.w = 1;
  label.h = 1;
  label.childClass = THDL_LABEL;
  label.parent = &secondW;

  THDL input;
  strcpy(input.wndTitle, "Input");
  strcpy(input.className, "Input");
  input.x = 1;
  input.y = 6;
  input.w = 13;
  input.h = 3;
  input.childClass = THDL_INPUT;
  input.thdlInputCallback = onInput;
  input.parent = &secondW;

  symbols.appendWindow(&secondW);
  symbols.appendWindow(&label);
  symbols.appendWindow(&input);

  while(1){}; 
}
