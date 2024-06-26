# Api documentation
To start off, you need to know that every TWn application is a .so file, because I didn't found another way to implement api<br>
How to compile: ```gcc app.c -shared -fPIC -o app.so```
## window.h
Type definitions for window, does not contain any functions.
```c++
typedef struct THDL {
  char wndTitle[100];    // window title
  char className[100];   // window class
  int x;                // window position by x
  int y;                // window position by y
  int w;                // window width
  int h;                // window height
  int uid;              // numeric identifier for window
  struct THDL* parent;  // parent window, if not NULL, window will be defined as a children element
  void (*thdlCallback)(struct THDL*,  int); // function ran on message (e.g mouse click)
  void (*thdlInputCallback)(struct THDL*, char*); // function ran on input (when text typed)
  int childClass;       // element class (e.g button/input)
  int winflags;         // window flags
} THDL;

// messages that are passed to second argument on thdlCallback
enum CALLBACK_MESSAGES {
  CALLBACK_CLICKED = 0x0001,
};

// thdl clildclasses, used to determine what callback to use, or how to render the element
enum THDL_CLILDCLASSES {
  THDL_BUTTON = 0x0001, 
  THDL_INPUT  = 0x0002,
  THDL_LABEL  = 0x0003,
};

enum THDL_WINFLAGS {
  THDL_NORESIZE = 0x0001,
};
```

## windowStore.h
This file contains every function to interact with window(s), and here are they

```c++
std::vector<THDL*> window_store; // variable that stores every window and it's children
void appendWindow(THDL* window); // add window to window_store
void removeWindowByTitle(const char* title); // remove window from window_store by title
void removeWindowByUID(int uid); // remove window from window_store by uid
THDL* getWindowByTitle(const char* title);   // get window from window_store by title
THDL* getWindowByIndex(size_t idx); // get window from window_store by index
size_t getWindowStoreLength()       // get length of window_store
```

## apiSymbolsStore.h
This file contains a struct for handling api symbols

```c
struct apiSymbolsStore {
 // windowStore functions
 void (*appendWindow)(THDL*);
 void (*removeWindowByTitle)(const char*);
 void (*removeWindowByUID)(int);
 THDL* (*getWindowByTitle)(const char*);
 THDL* (*getWindowByIndex)(size_t);
 size_t (*getWindowStoreLength)();
};
```

# Examples
## Simple window
```c
#include "./window.h"
#include "./apiSymbolsStore.h"
#include <string.h>

void *twnEntry(void *sym) // defines the entry point for application to start
{
  struct apiSymbolsStore symbols;
  symbols = *(struct apiSymbolsStore*)sym; // get symbols passed to (void* sym)
  THDL secondW;                            // create window
  strcpy(secondW.wndTitle, "Api window");  // set window title
  strcpy(secondW.className, "Api window"); // set window class
  secondW.x = 50;
  secondW.y = 30;
  secondW.w = 10;
  secondW.h = 10;
  symbols.appendWindow(&secondW);
  while(1){}; // IMPORTANT: Place this infinite loop at the end of twnEntry to not unload windows you created, or you'll get segmentation fault. This loop will not stop TWn from execution, because twnEntry runs in a thread
}
```
### Buttons
```c
#include "./window.h"
#include "./apiSymbolsStore.h"
#include <string.h>

int counter = 0;

void onButtonClick(THDL* sender, int message){
  if(message == CALLBACK_CLICKED){
    counter++;
    snprintf(sender->wndTitle, 100, "Counter: %d", counter);
  }
}

void onButton2Click(THDL* sender, int message){
  if(message == CALLBACK_CLICKED){
    sender->parent->x += 1;
    sender->parent->y += 1;
  }
}

void *twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;
  symbols = *(struct apiSymbolsStore*)sym;

  THDL secondW;
  strcpy(secondW.wndTitle, "Api window");
  strcpy(secondW.className, "Api window");
  secondW.x = 50;
  secondW.y = 30;
  secondW.w = 20;
  secondW.h = 10;
  secondW.winflags = THDL_NORESIZE;

  THDL button;
  strcpy(button.wndTitle, "Counter: 0");
  strcpy(button.className, "Counter");
  button.x = 1;
  button.y = 2;
  button.w = 16;
  button.h = 3;
  button.childClass = THDL_BUTTON;
  button.thdlCallback = onButtonClick;
  button.parent = &secondW;

  THDL button2;
  strcpy(button2.wndTitle, "Move window");
  strcpy(button2.className, "Move window");
  button2.x = 1;
  button2.y = 6;
  button2.w = 13;
  button2.h = 3;
  button2.childClass = THDL_BUTTON;
  button2.thdlCallback = onButton2Click;
  button2.parent = &secondW;

  symbols.appendWindow(&secondW);
  symbols.appendWindow(&button);
  symbols.appendWindow(&button2);

  while(1){}; 
}
```
### Input
```c
#include "./window.h"
#include "./apiSymbolsStore.h"
#include <string.h>

THDL label;

void onInput(THDL* sender, char* text){
  snprintf(label.wndTitle, 100, "Text typed: %s", text); // update label text
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
  secondW.winflags = THDL_NORESIZE;

  strcpy(label.wndTitle, "Text typed: ");
  strcpy(label.className, "typed");
  label.x = 1;
  label.y = 2;
  label.w = 1; // important: label need to have width and height >= 1
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
```
