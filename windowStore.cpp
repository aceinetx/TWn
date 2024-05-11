#include <vector>
#include <cstring>
#include "window.h"

inline std::vector<THDL*> window_store;

void appendWindow(THDL* window){
  window_store.push_back(window);
}

void removeWindowByTitle(const char* title){
  int remove_idx = -1;
  for(int i=0; i<window_store.size(); i++){
    THDL *wnd = window_store.at(i);
    if(strcmp(wnd->wndTitle, title) != 0){
      remove_idx = i;
      break;
    }
  }
  window_store.erase(window_store.begin() + remove_idx);
}

THDL* getWindowByTitle(const char* title){
  for(int i=0; i<window_store.size(); i++){
    THDL *wnd = window_store.at(i);
    if(strcmp(wnd->wndTitle, title) != 0){
      return wnd;
    }
  }
  return NULL;
}

THDL* getWindowByIndex(size_t idx){
  for(size_t i=0; i<window_store.size(); i++){
    if(i == idx){
      return window_store.at(i);
    }
  }
  return NULL;
}

size_t getWindowStoreLength(){
  return window_store.size();
}
