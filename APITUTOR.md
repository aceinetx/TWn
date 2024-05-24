# API Tutorial
It is highly recommended to read [api documentation](APIDOCS.md) before reading this tutorial.
## Include headers
In your .c file you need to include these headers:
```c
#include "./apiSymbolsStore.h"
#include "./window.h"
#include <string.h>
```
These are necessary to use the api
## Entry point
This function determines where your application will start. In arguments it passes a symbols structure with pointers to api functions
```c
THDL label;
void* twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;  // where we will store api functions
  symbols = *(struct apiSymbolsStore*)sym;
}
```
## Creating a window
```c
THDL window;
strcpy(window.wndTitle, "API Window");
strcpy(window.className, "APIWindow");
window.x = 1;
window.y = 1;
window.w = 20;
window.h = 10;
```
What's a THDL?: it is a structure with information required to properly render and make functionality for window/element<br>
## Adding window
If you run your code at this point, nothing will appear on the screen, we'll fix that now
```c
symbols.appendWindow(&window);
```
## Adding window children
This segment explains how to add ui elements to window
```c
THDL button;
strcpy(button.wndTitle, "Click me");
strcpy(button.className, "Button1");
button.x = 2;
button.y = 2;
button.w = 10;
button.h = 3;
button.childClass = THDL_BUTTON; // child class determines which type of element it is
button.parent = &window;
symbols.appendWindow(&button);

strcpy(label.wndTitle, "Counter: 0");
strcpy(label.className, "CounterLabel");
label.x = 2;
label.y = 6;
label.w = 10;
label.h = 3;
label.childClass = THDL_LABEL;
label.parent = &window;
symbols.appendWindow(&label);
```
## Callback
Let's make functionality!. We'll add a callback to button when clicked
```c
int counter=0; // counter
void ButtonCallback(THDL* sender, int message){
  if(message == CALLBACK_CLICKED){
    counter++; // counter goes ++
    snprintf(label.wndTitle, 100, "Counter: %d", counter); // 100 is maximum length for wndTitle and className
  }
}

button.thdlCallback = ButtonCallback; // add callback to button
```

## !! IMPORTANT
place this code at the end of twnEntry function:
```c
while(1){}
```
This is needed to keep your application running, if you don't do that TWn will crash
## Final code
Final code should look like that
```c
#include "./apiSymbolsStore.h"
#include "./window.h"
#include <string.h>

THDL label;
int counter=0; // counter
void ButtonCallback(THDL* sender, int message){
  if(message == CALLBACK_CLICKED){
    counter++; // counter goes ++
    snprintf(label.wndTitle, 100, "Counter: %d", counter); // 100 is maximum length for wndTitle and className
  }
}

void* twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;  // where we will store api functions
  symbols = *(struct apiSymbolsStore*)sym;

  THDL window;
  strcpy(window.wndTitle, "API Window");
  strcpy(window.className, "APIWindow");
  window.x = 1;
  window.y = 1;
  window.w = 20;
  window.h = 10;
  symbols.appendWindow(&window);

  THDL button;
  strcpy(button.wndTitle, "Click me");
  strcpy(button.className, "Button1");
  button.x = 2;
  button.y = 2;
  button.w = 10;
  button.h = 3;
  button.childClass = THDL_BUTTON; // child class determines which type of element it is
  button.parent = &window;
  symbols.appendWindow(&button);
  button.thdlCallback = ButtonCallback; // add callback to button
  
  strcpy(label.wndTitle, "Counter: 0");
  strcpy(label.className, "CounterLabel");
  label.x = 2;
  label.y = 6;
  label.w = 10;
  label.h = 3;
  label.childClass = THDL_LABEL;
  label.parent = &window;
  symbols.appendWindow(&label);

  while(1){}
}
```

## Let's compile
gcc -o app.so app.c -shared -fPIC<br>
nothing special
## Testing
Open the TWn, press Z and type "./yourcompiledappname". If you've done everything right, you should see a window with everything we added to it
