#include "Gbl.h"

void Gbl::freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  int freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  strPtr->print(F("Free Memory: "));
  strPtr->println(freeRam);
}
