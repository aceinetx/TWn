#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <cstring>
#include "window.h"
#include "windowStore.h"
#include <stdbool.h>

THDL* movingWindow = NULL;
bool moving_window = false;
bool debugInfo = false;

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

  THDL secondW;
  strcpy(secondW.wndTitle, "Second window");
  strcpy(secondW.className, "Second window");
  secondW.x = 30;
  secondW.y = 20;
  secondW.w = 10;
  secondW.h = 10;

  mousemask(ALL_MOUSE_EVENTS, NULL);

  appendWindow(&mainWindow);
  appendWindow(&secondW);

  MEVENT mouseEvent;

  while(true){
    int max_x = getmaxx(stdscr);
    int max_y = getmaxy(stdscr);

    for(int i=0; i<max_y; i++){
      for(int j=0; j<max_x; j++){
        mvprintw(i, j, ".");
      }
    }


    for(size_t i=0; i<getWindowStoreLength(); i++){
      THDL *thdl = getWindowByIndex(i);
      for(int j=thdl->y; j<=thdl->h+thdl->y; j++){
        for(int k=thdl->x; k<=thdl->w+thdl->x; k++){
          mvprintw(j, k, "#");
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

    if(debugInfo){
      mvprintw(0, 0, "%d %d %d", mouseEvent.y, mouseEvent.x, moving_window);
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
          if(mouseEvent.x >= thdl->x && (mouseEvent.x <= thdl->x+strlen(thdl->wndTitle) && (mouseEvent.y == thdl->y))){
          //if(true){
            movingWindow = thdl;
            moving_window = true;
            cancelMove = true;
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
