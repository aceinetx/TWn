# Api documentation
To start off, you need to know that every TWn application is a .so file, because I didn't found another way to implement api
## window.h
Type definitions for window, does not contain any functions.
```c++
typedef struct THDL {
  char wndTitle[25];    // window title
  char className[25];   // window class
  int x;                // window position by x
  int y;                // window position by y
  int w;                // window width
  int h;                // window height
  struct THDL* parent;  // parent window, if not NULL, window will be defined as a children element
  void (*thdlCallback)(struct THDL*,  int); // function ran on message (e.g mouse click)
  void (*thdlInputCallback)(struct THDL*, char*); // function ran on input (when text typed)
  int childClass;       // element class (e.g button/input)
} THDL;

// messages that are passed to second argument on thdlCallback
enum CALLBACK_MESSAGES {
  CALLBACK_CLICKED = 0x0001,
};

// thdl clildclasses, used to determine what callback to use, or how to render the element
enum THDL_CLILDCLASSES {
  THDL_BUTTON = 0x0001, 
  THDL_INPUT  = 0x0002,
};
```

## windowStore.h
This file contains every function to interact with window(s), and here are they

```c++
std::vector<THDL*> window_store; // variable that stores every window and it's children
void appendWindow(THDL* window); // add window to window_store
void removeWindowByTitle(const char* title); // remove window from window_store by title
THDL* getWindowByTitle(const char* title);   // get window from window_store by title
THDL* getWindowByIndex(size_t idx); // get window from window_store by index
size_t getWindowStoreLength()       // get length of window_store
```
