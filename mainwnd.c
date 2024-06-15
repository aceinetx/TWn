#include "./apiSymbolsStore.h"
#include "./window.h"
#include <string.h>
#include <ncurses.h>

THDL wnd;
struct apiSymbolsStore symbols;

THDL *getMainWnd(){
  return &wnd;
}

void openFiles(THDL* sender, int message){
  if(message == CALLBACK_CLICKED){
    if(sender->uid == -53021){
      symbols.runTWnApplication("./files.so");
    } else if(sender->uid == -53022){
      symbols.runTWnApplication("./calc.so");
    }
  }
}

void* twnEntry(void* sym){
  symbols = *(struct apiSymbolsStore*)sym;

  strcpy(wnd.wndTitle, "TWn Main Window");
  strcpy(wnd.className, "MainWnd");
  wnd.h = 10;
  wnd.w = 20;
  wnd.x = 0;
  wnd.y = -1;
  wnd.uid = -53020;
  wnd.hidden = 1;
  symbols.appendWindow(&wnd);

  THDL filesbtn;
  strcpy(filesbtn.wndTitle, "Files");
  strcpy(filesbtn.className, "filesbtn");
  filesbtn.x = 1;
  filesbtn.y = 2;
  filesbtn.w = 7;
  filesbtn.h = 3;
  filesbtn.parent = &wnd;
  filesbtn.childClass = THDL_BUTTON;
  filesbtn.thdlCallback = openFiles;
  filesbtn.uid = -53021;
  symbols.appendWindow(&filesbtn);

  THDL calcbtn;
  strcpy(calcbtn.wndTitle, "Calculator");
  strcpy(calcbtn.className, "calcbtn");
  calcbtn.x = 1;
  calcbtn.y = 6;
  calcbtn.w = 12;
  calcbtn.h = 3;
  calcbtn.parent = &wnd;
  calcbtn.childClass = THDL_BUTTON;
  calcbtn.thdlCallback = openFiles;
  calcbtn.uid = -53022;
  symbols.appendWindow(&calcbtn);

  while(1){
    wnd.desktop_id = symbols.getDesktopId();
  }
}
