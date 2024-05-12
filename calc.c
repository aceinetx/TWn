#include "./apiSymbolsStore.h"
#include "./window.h"
#include <string.h>

THDL label;

float firstnum  = 0;
char  operator  = '+';
float secondnum = 0;
float result;

void calculate(){
  result = 0;
  switch(operator){
    case '+': result = firstnum+secondnum; break;
    case '-': result = firstnum-secondnum; break;
    case '*': result = firstnum*secondnum; break;
    case '/': result = firstnum/secondnum; break;
  }
}

void makeFinalResult(){
  snprintf(label.wndTitle, 100, "Final result: %f %c %f = %f", firstnum, operator, secondnum, result);
}

void onInputDone(THDL* sender, char* text){
  if(sender->uid == -53010){
    firstnum = atof(text);
  } else if(sender->uid == -53011){
    if(text[0] == '+' || text[0] == '-' || text[0] == '/' || text[0] == '*'){
      operator = text[0];
    }
  } else if(sender->uid == -53012){
    secondnum = atof(text);
  }
  calculate();
  makeFinalResult();
}

void* twnEntry(void *sym)
{
  struct apiSymbolsStore symbols;  // where we will store api functions
  symbols = *(struct apiSymbolsStore*)sym;

  THDL window;
  strcpy(window.wndTitle, "TWn Calculator");
  strcpy(window.className, "TWnCalculator");
  window.x = 1;
  window.y = 1;
  window.w = 70;
  window.h = 15;
  window.winflags = THDL_NORESIZE;
  symbols.appendWindow(&window);

  strcpy(label.wndTitle, "Final result:");
  strcpy(label.className, "ResultLabel");
  label.x = 2;
  label.y = 2;
  label.w = 20;
  label.h = 1;
  label.parent = &window;
  label.childClass = THDL_LABEL;
  symbols.appendWindow(&label);

  THDL input1;
  strcpy(input1.wndTitle, "First number");
  strcpy(input1.className, "Input1");
  input1.x = 2;
  input1.y = 4;
  input1.w = 20;
  input1.h = 3;
  input1.parent = &window;
  input1.childClass = THDL_INPUT;
  input1.uid = -53010;
  input1.thdlInputCallback = onInputDone;
  symbols.appendWindow(&input1);

  THDL input2;
  strcpy(input2.wndTitle, "Operator");
  strcpy(input2.className, "Input2");
  input2.x = 2;
  input2.y = 8;
  input2.w = 20;
  input2.h = 3;
  input2.parent = &window;
  input2.childClass = THDL_INPUT;
  input2.uid = -53011;
  input2.thdlInputCallback = onInputDone;
  symbols.appendWindow(&input2);

  THDL input3;
  strcpy(input3.wndTitle, "Second number");
  strcpy(input3.className, "Input3");
  input3.x = 2;
  input3.y = 12;
  input3.w = 20;
  input3.h = 3;
  input3.parent = &window;
  input3.childClass = THDL_INPUT;
  input3.uid = -53012;
  input3.thdlInputCallback = onInputDone;
  symbols.appendWindow(&input3);


  makeFinalResult();
  while(1){}
} 
