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

THDL* movingWindow = NULL;
bool moving_window = false;
bool debugInfo = false;
char latestError[256];

typedef void *(*twnEntry_f)(void*);

char graphics[] = {'.', '#', ':'};

bool runTWnApplication(char* fileName){

  struct apiSymbolsStore symbols;
  symbols.appendWindow = appendWindow; 
  symbols.removeWindowByTitle = removeWindowByTitle;
  symbols.getWindowByTitle = getWindowByTitle;
  symbols.getWindowByIndex = getWindowByIndex;
  symbols.getWindowStoreLength = getWindowStoreLength;

  twnEntry_f twnEntry;
  void *handle;
  handle = dlopen(fileName, RTLD_NOW);
  if(handle){
    twnEntry = (twnEntry_f)dlsym(handle, "twnEntry");
    if (dlerror() != NULL){
      snprintf(latestError, 256, "Error loading twn application: %s", dlerror());
    } else {
      pthread_t dlth;
      pthread_create(&dlth, NULL, twnEntry, &symbols);
    }
  }
}

int main(int argc, char** argv){
  initscr();

  noecho();
  curs_set(0);
  start_color();
  use_default_colors();

  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  THDL mainWindow;
  strcpy(mainWindow.wndTitle, "Test window");
  strcpy(mainWindow.className, "Test window");
  mainWindow.x = 1;
  mainWindow.y = 1;
  mainWindow.w = 10;
  mainWindow.h = 5;

  mousemask(ALL_MOUSE_EVENTS, NULL);
  
  appendWindow(&mainWindow);

  runTWnApplication("./testApi.so");

  MEVENT mouseEvent;

  while(true){
    int max_x = getmaxx(stdscr);
    int max_y = getmaxy(stdscr);

    for(int i=0; i<max_y; i++){
      for(int j=0; j<max_x; j++){
        mvprintw(i, j, "%c", graphics[0]);
      }
    }


    for(size_t i=0; i<getWindowStoreLength(); i++){
      THDL *thdl = getWindowByIndex(i);
      if(thdl->parent == NULL){
        for(int j=thdl->y; j<=thdl->h+thdl->y; j++){
          for(int k=thdl->x; k<=thdl->w+thdl->x; k++){
            mvprintw(j, k, "%c", graphics[1]);
          }
        }

        char* newTitle;
        if(movingWindow == thdl){
          newTitle = (char*)malloc(strlen(thdl->wndTitle)+3);
          snprintf(newTitle, strlen(thdl->wndTitle)+3, "o %s", thdl->wndTitle);
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
              mvprintw(j, k, "%c", graphics[2]);
              int text_x = thdl->x+parent->x+(thdl->w/2)-(strlen(thdl->wndTitle)/2);
              int text_y = thdl->y+parent->y+(thdl->h/2);
              mvprintw(text_y, text_x, "%s", thdl->wndTitle);
            }
          }
        }
      }
    }

    if(debugInfo){
      mvprintw(0, 0, "%d %d %d %s", mouseEvent.y, mouseEvent.x, moving_window, latestError);
    }

    refresh();

    int k=getch();
    if(k == 'q') break;
    else if(k == KEY_MOUSE){
      getmouse(&mouseEvent);
      bool cancelMove = false;

      if(!moving_window){
        for(size_t i=0; i<getWindowStoreLength(); i++){
          THDL *thdl = getWindowByIndex(i);
          if(thdl->parent == NULL){
            if(mouseEvent.x >= thdl->x && (mouseEvent.x <= -1+thdl->x+strlen(thdl->wndTitle) && (mouseEvent.y == thdl->y))){
              movingWindow = thdl;
              moving_window = true;
              cancelMove = true;
            }
          } else {
            if(mouseEvent.x >= (thdl->x+thdl->parent->x) && (mouseEvent.x <= -1+thdl->x+thdl->parent->x+thdl->w) && (mouseEvent.y >= (thdl->y+thdl->parent->y) && (mouseEvent.y <= (-1+thdl->y+thdl->parent->y+thdl->h)))){
              
              if(thdl->thdlCallback != NULL) thdl->thdlCallback(thdl, CALLBACK_CLICKED);
              if(thdl->childClass == THDL_INPUT){
                char buf[256];
                nodelay(stdscr, FALSE);
                echo();
                curs_set(1);
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

      if(moving_window && (!cancelMove)){
        moving_window = false;


        movingWindow->x = mouseEvent.x;
        movingWindow->y = mouseEvent.y;
        

        movingWindow = NULL;
      }
    } else if(k == 'p'){
      debugInfo = !debugInfo;
    }
  }

  endwin();
}
