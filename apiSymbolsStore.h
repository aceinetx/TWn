#pragma once
#include "./window.h"

struct apiSymbolsStore {
  // windowStore functions
 void (*appendWindow)(THDL*);
 void (*removeWindowByTitle)(const char*);
 void (*removeWindowByUID)(int);
 THDL* (*getWindowByTitle)(const char*);
 THDL* (*getWindowByIndex)(size_t);
 size_t (*getWindowStoreLength)();

};
