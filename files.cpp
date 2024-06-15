#include "./apiSymbolsStore.h"
#include "./window.h"
#include "./windowStore.h"
#include <string.h>
#include <iostream>
#include <bits/stdc++.h>

#define FS_EXPERIMENTAL

#ifdef FS_EXPERIMENTAL
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
#else
#include <filesystem>

namespace fs = std::filesystem;
#endif

#define FILES_LENGTH 30

THDL labelPath;
THDL files[FILES_LENGTH];

void emptyCallback(THDL* sender, int message) {}

void changeDir(THDL* sender, int message){
  if(message == CALLBACK_MESSAGES::CALLBACK_CLICKED && sender->wndTitle[0] != 0){
    std::string new_path;
    new_path.append(labelPath.wndTitle);
    new_path.append(sender->wndTitle);
    new_path.push_back('/');
    if(new_path.length() < sizeof(sender->wndTitle)){
      memcpy(labelPath.wndTitle, new_path.c_str(), sizeof(sender->wndTitle)-1);
    }
  }
  
  for(int i=0; i<FILES_LENGTH; i++){
    THDL *wnd = &files[i];
    wnd->thdlCallback = emptyCallback;
    memset(wnd->wndTitle, 0, 99);
  }

  int i=0;
  for(const auto & entry : fs::directory_iterator(labelPath.wndTitle)){
    if(i >= FILES_LENGTH) break;
    THDL* v = &files[i];
    memset(v->wndTitle, 0, sizeof(v->wndTitle));
    if(strlen(entry.path().filename().c_str()) < 100)
    memcpy(v->wndTitle, entry.path().filename().c_str(), strlen(entry.path().filename().c_str()));
    
    if(fs::is_directory(entry)){
    v->thdlCallback = changeDir;
    } else {
    v->thdlCallback = emptyCallback;
    }
    i++;
  }
}

void upDir(THDL* sender, int message){
  if(labelPath.wndTitle[0] == '/' && labelPath.wndTitle[1] == 0) return;
  std::string string_path;
  bool allow = false;
  for(int i=strlen(labelPath.wndTitle)-2; i>=0; i--){
    if(labelPath.wndTitle[i] == '/') allow = true;
    if(allow) string_path.push_back(labelPath.wndTitle[i]);
  }
  std::reverse(string_path.begin(), string_path.end());
  memcpy(labelPath.wndTitle, string_path.c_str(), 99);

  for(int i=0; i<FILES_LENGTH; i++){
    THDL *wnd = &files[i];
    wnd->thdlCallback = emptyCallback;
    memset(wnd->wndTitle, 0, 99);
  }

  int i=0;
  for(const auto & entry : fs::directory_iterator(labelPath.wndTitle)){
    if(i >= FILES_LENGTH) break;
    THDL* v = &files[i];
    memset(v->wndTitle, 0, sizeof(v->wndTitle));
    if(strlen(entry.path().filename().c_str()) < 100)
    memcpy(v->wndTitle, entry.path().filename().c_str(), strlen(entry.path().filename().c_str()));
    v->thdlCallback = changeDir;
    i++;
  }
}

extern "C" {
void* twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;  // where we will store api functions
  symbols = *(struct apiSymbolsStore*)sym;

  THDL window;
  strcpy(window.wndTitle, "Files");
  strcpy(window.className, "files");
  window.x = 10;
  window.y = 10;
  window.w = 70;
  window.h = FILES_LENGTH+4;
  window.winflags = THDL_NORESIZE;
  symbols.appendWindow(&window);

  strcpy(labelPath.wndTitle, "/");
  strcpy(labelPath.className, "labelpath");
  labelPath.x = 1;
  labelPath.y = 2;
  labelPath.w = window.w-1; labelPath.h = 1;
  labelPath.childClass = THDL_LABEL;
  labelPath.parent = &window;
  labelPath.thdlCallback = upDir;
  symbols.appendWindow(&labelPath);

  for(int i=0; i<FILES_LENGTH; i++){
    THDL *wnd = &files[i];
    wnd->x = 1;
    wnd->y = 4+i;
    wnd->w = window.w-2; wnd->h = 1;
    wnd->childClass = THDL_LABEL;
    wnd->parent = &window;
    symbols.appendWindow(&files[i]);
  }

  for(int i=0; i<FILES_LENGTH; i++){
    THDL *wnd = &files[i];
    wnd->thdlCallback = emptyCallback;
    memset(wnd->wndTitle, 0, 99);
  }

  int i=0;
  for(const auto & entry : fs::directory_iterator(labelPath.wndTitle)){
    if(i >= FILES_LENGTH) break;
    THDL* v = &files[i];
    memset(v->wndTitle, 0, sizeof(v->wndTitle));
    if(strlen(entry.path().filename().c_str()) < 100)
    memcpy(v->wndTitle, entry.path().filename().c_str(), strlen(entry.path().filename().c_str()));
    v->thdlCallback = changeDir;
    i++;
  }

  while(1){}
} 
}
