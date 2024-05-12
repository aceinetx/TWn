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
void* twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;  // where we will store api functions
  symbols = *(struct apiSymbolsStore*)sym;
}
```
## Creating a window
```c
THDL window;
strcpy(window.wndTitle, "
```
What's a THDL?: it is a structure with information required to properly render and make functionality for window<br>


## !! IMPORTANT
place this code at the end of twnEntry function:
```c
while(1){}
```
This is needed to keep your application running, if you don't do that TWn will crash
