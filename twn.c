#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "./window.h"
#include "./windowStore.h"
#include <stdbool.h>
#include <dlfcn.h>
#include "./apiSymbolsStore.h"
#include <pthread.h>
#include <locale.h>
#include "./TWNAPPLICATIONRESULT.h"

#define BUTTON_CLICKED_FRAMES_VAR 200
#define MAX_DESKTOP_ID 3
#define ctrl(x) ((x) & 0x1f)

THDL* movingWindow = NULL;
bool moving_window = false;
bool debugInfo = false;
char latestError[256];
bool bottombar = true;

char desktop_id = 0;

typedef void *(*twnEntry_f)(void*);

wchar_t graphics[] = {L'▒', L'▓', L'▒'};

struct apiSymbolsStore symbols;

struct TWnApplicationResult runTWnApplication(const char* fileName){
  struct TWnApplicationResult res;
  res.handle = NULL;
  res.success = true;
  twnEntry_f twnEntry;
  void *handle;
  handle = dlopen(fileName, RTLD_NOW);
  if(handle){
    res.handle = handle;
    twnEntry = (twnEntry_f)dlsym(handle, "twnEntry");
    if (dlerror() != NULL){
      snprintf(latestError, 256, "Error loading twn application: %s", dlerror());
      res.success = false;
      return res;
    } else {
      pthread_t dlth;
      pthread_create(&dlth, NULL, twnEntry, &symbols);
    }
  } else {
    res.success = false;
    return res;
  }
  return res;
}

THDL quickstartWindow;
void welcomeWindowCallback(THDL* sender, int message){
  if(message == CALLBACK_CLICKED){
    quickstartWindow.hidden = !quickstartWindow.hidden;
  }
}

bool first_frame_rendered = false;

void initScreen(){
  initscr();
  noecho();
  curs_set(0);
  start_color();
  use_default_colors();
  cbreak();

  mouseinterval(1);

  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
}

char getDesktopId(){
  return desktop_id;
}

int main(int argc, char** argv){
  srand(time(NULL));
  symbols.appendWindow = appendWindow; 
  symbols.removeWindowByTitle = removeWindowByTitle;
  symbols.getWindowByTitle = getWindowByTitle;
  symbols.getWindowByIndex = getWindowByIndex;
  symbols.getWindowStoreLength = getWindowStoreLength;
  symbols.removeWindowByUID = removeWindowByUID;
  symbols.runTWnApplication = runTWnApplication;
  symbols.getDesktopId = getDesktopId;

  system("clear");

  setlocale(LC_ALL, "");

  initScreen();

  for(int i=0; i<COLORS; i++){
    init_pair(i+1, i, -1);
  }

  init_pair(501, 249, 249);

  mousemask(BUTTON1_PRESSED | BUTTON2_PRESSED | BUTTON2_RELEASED, NULL);
  
  struct TWnApplicationResult mainWndRes = runTWnApplication("./mainwnd.so");

  MEVENT mouseEvent;
  mouseEvent.x = 0;
  mouseEvent.y = 0;

  int fcx = 0;
  int fcy = 0;
  bool cur_kb_mode = false;

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
      if(thdl == NULL) continue;
      if(thdl->parent == NULL && (thdl->hidden == 0) && (thdl->desktop_id == desktop_id)){
        for(int j=thdl->y; j<=thdl->h+thdl->y; j++){
          for(int k=thdl->x; k<=thdl->w+thdl->x; k++){
            attron(COLOR_PAIR(237));
            mvaddwstr(j, k, &graphics[1]);
            attroff(COLOR_PAIR(237));

            attron(COLOR_PAIR(2));
            mvprintw(thdl->y, thdl->x+thdl->w, "▒");
            attroff(COLOR_PAIR(2));
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
      } else {
        if(thdl->parent != NULL){
          thdl->hidden = thdl->parent->hidden;
          thdl->desktop_id = thdl->parent->desktop_id;
        }
      }
    }

    for(size_t i=0; i<getWindowStoreLength(); i++){
      THDL *thdl = getWindowByIndex(i);
      if(thdl == NULL) continue;
      if(thdl->parent != NULL && (thdl->hidden == 0) && (thdl->desktop_id == desktop_id)){
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
            } else if(thdl->childClass == THDL_CHECKBOX){
              if(thdl->checkboxChecked){
                mvprintw(thdl->y+parent->y, thdl->x+parent->x, "[x] %s", thdl->wndTitle);
              } else {
                mvprintw(thdl->y+parent->y, thdl->x+parent->x, "[ ] %s", thdl->wndTitle);
              }
            }
          }
        }
      }
    }

    if(debugInfo){
      mvprintw(0, 0, "%d %d %d %s %ld %d", mouseEvent.y, mouseEvent.x, moving_window, latestError, getWindowStoreLength(), desktop_id);
    }

    if(cur_kb_mode){
      mvprintw(fcy, fcx, "*");
    }

    if(bottombar){

      for(int i=0; i<max_x; i++){
        mvprintw(max_y-1, i, " ");
      }

      float wndUsage=0;
      

      for(size_t i=0; i<getWindowStoreLength(); i++){
        THDL* wnd = getWindowByIndex(i);
        if(wnd->uid <= -53020 && (wnd->uid >= -53029)){
          
        } else {
          wndUsage += (float)sizeof(*wnd);
        }
      }

      char buf[100];
      snprintf(buf, sizeof(buf), "TWn (WndUsage: %fkB)", wndUsage/1000);
      mvprintw(max_y-1, 0, "%s", buf);
      for(char i=0; i<=MAX_DESKTOP_ID; i++){
        if(i != desktop_id) attron(COLOR_PAIR(249));
        mvprintw(max_y-1, i*2+strlen(buf)+1, "•");
        if(i != desktop_id) attroff(COLOR_PAIR(249));
      }
    }

    refresh();

    bool cancelMove = false;

    int k=wgetch(stdscr);

    bool cur_pressed = (k == KEY_MOUSE);
    if(cur_kb_mode){
      if(k == 'u'){
        cur_pressed = true;
        
      }
    }
    if(k == 'h') fcx -= 1;
    if(k == 'l') fcx += 1;
    if(k == 'j') fcy += 1;
    if(k == 'k') fcy -= 1;

    if(k == 'o') cur_kb_mode = !cur_kb_mode;

    if(moving_window && (movingWindow != NULL)){
      if(!(movingWindow->winflags & THDL_NORESIZE)){
        if(k == 'd') movingWindow->w++;
        if(k == 's') movingWindow->h++;
        if(k == 'w') movingWindow->h--;
        if(k == 'a') movingWindow->w--;
      }
      THDL* thdl = movingWindow;
      if(k == KEY_RIGHT){
        thdl->desktop_id += 1;
        if(thdl->desktop_id > MAX_DESKTOP_ID) thdl->desktop_id = MAX_DESKTOP_ID;
        movingWindow = NULL;
        moving_window = false;
      } else if(k == KEY_LEFT){
        thdl->desktop_id -= 1;
        if(thdl->desktop_id < 0) thdl->desktop_id = 0;
        movingWindow = NULL;
        moving_window = false;
      }
    } else {
      if(k == KEY_RIGHT){
        desktop_id += 1;
        if(desktop_id > MAX_DESKTOP_ID) desktop_id = MAX_DESKTOP_ID;
      } else if(k == KEY_LEFT){
        desktop_id -= 1;
        if(desktop_id < 0) desktop_id = 0;
      }
    }
    if(k == 'q') break;
    else if(cur_pressed && (first_frame_rendered)){
      getmouse(&mouseEvent);

      if(cur_kb_mode){
        mouseEvent.x = fcx; mouseEvent.y = fcy;
      }


      if(mouseEvent.bstate & BUTTON1_PRESSED || (mouseEvent.bstate & BUTTON2_PRESSED) || (mouseEvent.bstate & BUTTON2_RELEASED) || cur_kb_mode){

      if(!moving_window){
        for(size_t i=0; i<getWindowStoreLength(); i++){
          THDL *thdl = getWindowByIndex(i);
          if(thdl->parent == NULL){
            if(thdl->hidden == 0 && (thdl->desktop_id == desktop_id)){
            if(mouseEvent.x >= thdl->x && (mouseEvent.x <= -1+thdl->x+thdl->w && (mouseEvent.y == thdl->y))){
              movingWindow = thdl;
              moving_window = true;
              cancelMove = true;
            } else if(mouseEvent.x == (thdl->x+thdl->w) && (mouseEvent.y == thdl->y)){
                  thdl->uid = rand();
                  for(size_t j=0; j<getWindowStoreLength(); j++){
                    THDL* child = getWindowByIndex(j);
                    if(child->parent == thdl){
                      j--;
                      child->uid = rand();
                      removeWindowByUID(child->uid);
                    }
                  }
                  removeWindowByUID(thdl->uid);
                  
                }
            }
          } else {
            if(mouseEvent.x >= (thdl->x+thdl->parent->x) && (mouseEvent.x <= -1+thdl->x+thdl->parent->x+thdl->w) && (mouseEvent.y >= (thdl->y+thdl->parent->y) && (mouseEvent.y <= (-1+thdl->y+thdl->parent->y+thdl->h)))){

              if(thdl->hidden == 0 && (thdl->desktop_id == desktop_id)){
              if(thdl->childClass == THDL_BUTTON){
                thdl->BUTTON_CLICKED_FRAMES = BUTTON_CLICKED_FRAMES_VAR;
              }
              if(thdl->desktop_id == desktop_id){
                if(thdl->thdlCallback != NULL) thdl->thdlCallback(thdl, CALLBACK_CLICKED);
              }
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
                if(thdl->desktop_id == desktop_id){
                  if(thdl->thdlInputCallback != NULL) thdl->thdlInputCallback(thdl, buf);
                }
              }
              }
            }
              if(mouseEvent.y == thdl->y+thdl->parent->y){
                if(mouseEvent.x >= thdl->x+thdl->parent->x){
                  if(mouseEvent.x <= thdl->x+thdl->parent->x+2){
                    if(thdl->desktop_id == desktop_id){
                      thdl->checkboxChecked = !thdl->checkboxChecked;
                    }
                  }
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
    } else if(k == 'z'){
      char buf[256];
      nodelay(stdscr, FALSE);
      echo();
      curs_set(1);
      mvprintw(0, 0, "Open TWn application (path e.g: ./app.so): ");
      getnstr(buf, 256);
      nodelay(stdscr, TRUE);
      noecho();
      curs_set(0);
      runTWnApplication(buf);
    } else if(k == 'b'){
      bottombar = !bottombar;
    } else if(k == 'c'){
      endwin();
      printf("[terminal entered]\n");
      system(getenv("SHELL"));
      initScreen();
    } else if(k == 'x'){
      if(mainWndRes.success == true){
        typedef THDL *(*getMainWnd_f)();
        getMainWnd_f getMainWnd;
        getMainWnd = (getMainWnd_f)dlsym(mainWndRes.handle, "getMainWnd");
        THDL* wnd = getMainWnd();
        wnd->hidden = !wnd->hidden;
      }
    }
    first_frame_rendered = true;
  }

  endwin();
}
