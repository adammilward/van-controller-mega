#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-02-11 19:22:44

#include "Arduino.h"
#define DEBUG
#include <Arduino.h>
#include <stdlib.h>
#include "Controller.h"
#include "Gbl.h"
#include "Light.h"
extern Controller masterCtr;
extern Light Red;
extern Light Green;
extern Light Blue;
#define MEGA
#define BTUNO
#include "SoftwareSerial.h"
extern SoftwareSerial BT;
#define TME
#include <DS3231.h>
#include "TimeCtr.h"

void setup() ;
void loop() ;

#include "time.ino"


#endif
