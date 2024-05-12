#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <cstring>
#include "./window.h"
#include "./windowStore.h"
#include <stdbool.h>
#include <dlfcn.h>
#include "./apiSymbolsStore.h"
#include <pthread.h>
#include <locale.h>

#define BUTTON_CLICKED_FRAMES_VAR 200

THDL* movingWindow = NULL;
bool moving_window = false;
bool debugInfo = false;
char latestError[256];

typedef void *(*twnEntry_f)(void*);

wchar_t graphics[] = {L'▒', L'▓', L'▒'};

struct apiSymbolsStore symbols;

bool runTWnApplication(const char* fileName){
  twnEntry_f twnEntry;
  void *handle;
  handle = dlopen(fileName, RTLD_NOW);
  if(handle){
    twnEntry = (twnEntry_f)dlsym(handle, "twnEntry");
    if (dlerror() != NULL){
      snprintf(latestError, 256, "Error loading twn application: %s", dlerror());
      return false;
    } else {
      pthread_t dlth;
      pthread_create(&dlth, NULL, twnEntry, &symbols);
    }
  } else {
    return false;
  }
  return true;
}

int main(int argc, char** argv){
  symbols.appendWindow = appendWindow; 
  symbols.removeWindowByTitle = removeWindowByTitle;
  symbols.getWindowByTitle = getWindowByTitle;
  symbols.getWindowByIndex = getWindowByIndex;
  symbols.getWindowStoreLength = getWindowStoreLength;

  setlocale(LC_ALL, "");

  initscr();

  noecho();
  curs_set(0);
  start_color();
  use_default_colors();
  cbreak();

  mouseinterval(1);

  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  for(int i=0; i<COLORS; i++){
    init_pair(i+1, i, -1);
  }

  init_pair(501, 249, 249);

  THDL mainWindow;
  strcpy(mainWindow.wndTitle, "Test window");
  strcpy(mainWindow.className, "Test window");
  mainWindow.x = 1;
  mainWindow.y = 1;
  mainWindow.w = 10;
  mainWindow.h = 5;

  THDL child;
  strcpy(child.wndTitle, "Hi");
  strcpy(child.className, "Hi");
  child.x = 2;
  child.y = 2;
  child.w = 6;
  child.h = 3;
  child.parent = &mainWindow;
  child.childClass = THDL_BUTTON;

  mousemask(BUTTON1_PRESSED | BUTTON2_PRESSED | BUTTON2_RELEASED, NULL);
  
  appendWindow(&mainWindow);
  appendWindow(&child);

  runTWnApplication("./testApi.so");

  MEVENT mouseEvent;

  while(true){
    int max_x = getmaxx(stdscr);
    int max_y = getmaxy(stdscr);

    // drawing

    for(int i=0; i<max_y; i++){
      for(int j=0; j<max_x; j++){
        attron(COLOR_PAIR(242));
        mvaddwstr(i, j, L"▒");
        attroff(COLOR_PAIR(242));
      }
    }


    for(size_t i=0; i<getWindowStoreLength(); i++){
      THDL *thdl = getWindowByIndex(i);
      if(thdl->parent == NULL){
        for(int j=thdl->y; j<=thdl->h+thdl->y; j++){
          for(int k=thdl->x; k<=thdl->w+thdl->x; k++){
            attron(COLOR_PAIR(237));
            mvaddwstr(j, k, &graphics[1]);
            attroff(COLOR_PAIR(237));
          }
        }

        char* newTitle;
        if(movingWindow == thdl){
          newTitle = (char*)malloc(strlen(thdl->wndTitle)+1+strlen("• "));
          snprintf(newTitle, strlen(thdl->wndTitle)+strlen("• ")+1, "• %s", thdl->wndTitle);
        } else {
          newTitle = (char*)malloc(strlen(thdl->wndTitle+1));
          strcpy(newTitle, thdl->wndTitle);
        }

        mvprintw(thdl->y, thdl->x, "%s", newTitle);
        free(newTitle);
        newTitle = NULL;
      }
    }

    for(size_t i=0; i<getWindowStoreLength(); i++){
      THDL *thdl = getWindowByIndex(i);
      if(thdl->parent != NULL){
        THDL *parent = thdl->parent;
        for(int j=thdl->y+parent->y; j<thdl->h+thdl->y+parent->y; j++){
          for(int k=thdl->x+parent->x; k<thdl->w+thdl->x+parent->x; k++){
            if(thdl->childClass == THDL_BUTTON || thdl->childClass == THDL_INPUT){

              if(thdl->BUTTON_CLICKED_FRAMES > 0) thdl->BUTTON_CLICKED_FRAMES--;

              if(thdl->BUTTON_CLICKED_FRAMES > 0) attron(COLOR_PAIR(249));
              mvaddwstr(j, k, &graphics[2]);
              if(thdl->BUTTON_CLICKED_FRAMES > 0) attroff(COLOR_PAIR(249));

              if(k == thdl->x+parent->x || (j==thdl->y+parent->y)){
                if(thdl->BUTTON_CLICKED_FRAMES == 0) attron(COLOR_PAIR(249));
                mvaddwstr(j, k, &graphics[2]);
                if(thdl->BUTTON_CLICKED_FRAMES == 0) attroff(COLOR_PAIR(249));
              }
              int text_x = thdl->x+parent->x+(thdl->w/2)-(strlen(thdl->wndTitle)/2);
              int text_y = thdl->y+parent->y+(thdl->h/2);
              mvprintw(text_y, text_x, "%s", thdl->wndTitle);
            } else if(thdl->childClass == THDL_LABEL){
              mvprintw(thdl->y+parent->y, thdl->x+parent->x, "%s", thdl->wndTitle);
            }
          }
        }
      }
    }

    if(debugInfo){
      mvprintw(0, 0, "%d %d %d %s %ld", mouseEvent.y, mouseEvent.x, moving_window, latestError, getWindowStoreLength());
    }

    refresh();

    bool cancelMove = false;

    int k=wgetch(stdscr);

    if(moving_window && (movingWindow != NULL)){
      if(!(movingWindow->winflags & THDL_NORESIZE)){
        if(k == 'd') movingWindow->w++;
        if(k == 's') movingWindow->h++;
        if(k == 'w') movingWindow->h--;
        if(k == 'a') movingWindow->w--;
      }
    }
    if(k == 'q') break;
    else if(k == KEY_MOUSE){
      getmouse(&mouseEvent);

      if(mouseEvent.bstate & BUTTON1_PRESSED || (mouseEvent.bstate & BUTTON2_PRESSED) || (mouseEvent.bstate & BUTTON2_RELEASED)){

      if(!moving_window){
        for(size_t i=0; i<getWindowStoreLength(); i++){
          THDL *thdl = getWindowByIndex(i);
          if(thdl->parent == NULL){
            if(mouseEvent.x >= thdl->x && (mouseEvent.x <= -1+thdl->x+thdl->w && (mouseEvent.y == thdl->y))){
              movingWindow = thdl;
              moving_window = true;
              cancelMove = true;
            }
          } else {
            if(mouseEvent.x >= (thdl->x+thdl->parent->x) && (mouseEvent.x <= -1+thdl->x+thdl->parent->x+thdl->w) && (mouseEvent.y >= (thdl->y+thdl->parent->y) && (mouseEvent.y <= (-1+thdl->y+thdl->parent->y+thdl->h)))){
              if(thdl->childClass == THDL_BUTTON){
                thdl->BUTTON_CLICKED_FRAMES = BUTTON_CLICKED_FRAMES_VAR;
              }
              if(thdl->thdlCallback != NULL) thdl->thdlCallback(thdl, CALLBACK_CLICKED);
              if(thdl->childClass == THDL_INPUT){
                char buf[256];
                nodelay(stdscr, FALSE);
                echo();
                curs_set(1);
                mvprintw(0, 0, "Type text: ");
                getnstr(buf, 256);
                nodelay(stdscr, TRUE);
                noecho();
                curs_set(0);
                if(thdl->thdlInputCallback != NULL) thdl->thdlInputCallback(thdl, buf);
              }
            }
          }
        }
      }

      if(mouseEvent.bstate & BUTTON2_RELEASED){
        mouseinterval(1);
      }

      if(mouseEvent.bstate & BUTTON2_PRESSED){
        mouseinterval(250);
      }

      if(moving_window && (!cancelMove)){
        moving_window = false;


        movingWindow->x = mouseEvent.x;
        movingWindow->y = mouseEvent.y;
        

        movingWindow = NULL;

      }
      }
    } else if(k == 'p'){
      debugInfo = !debugInfo;
    }

    
  }

  endwin();
}
