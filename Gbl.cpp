#include "Gbl.h"

void Gbl::freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  int freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  Serial.print(F("Free Memory: "));
  Serial.println(freeRam);
}

bool Gbl::isNum(char *word) {
	boolean isNum=true;
    const char *p;
	p = word;
	while (*p) {
		isNum = (
			isDigit(*p) ||
			*p == '+' ||
			*p == '.' ||
			*p == '-'
		);
		if(!isNum) return false;
		p++;
	}
    return true;
}


