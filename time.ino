#define DEBUG

#include <Arduino.h>
#include <stdlib.h>
#include "Controller.h"
#include "Gbl.h"
//#include "Light.h"

Controller masterCtr;      // handles the remotes
Stream *Gbl::strPtr = &Serial;


//Light Red(Gbl::RED_PIN, 0);
//Light Green(Gbl::GREEN_PIN, 1);
//Light Blue(Gbl::BLUE_PIN, 2);

//#define MEGA
//#define BTUNO
#ifdef BTUNO
	#include "SoftwareSerial.h"
	SoftwareSerial BT = SoftwareSerial(12, 13); // RX,TX
#endif

#define TME
#ifdef TME
#include <DS3231.h>
#include "TimeCtr.h"
DS3231  Clock(SDA, SCL);
DS3231 *TimeCtr::clock = &Clock;
//Light *TimeCtr::red = &Red;
//Light *TimeCtr::green = &Green;
//Light *TimeCtr::blue = &Blue;
#endif

void setup()
{
	// Setup Serial connection
	Serial.begin(115200);
#ifdef MEGA
	Serial3.begin(115200);
#endif
#ifdef BTUNO
	BT.begin(115200); // RX,TX
#endif
#ifdef TME
	Clock.begin();
#endif
	Gbl::strPtr->println(F("we are programmed to receive"));
	Gbl::freeRam();
}


void loop()
{
	// for anything running on a timer
	masterCtr.timer(millis());

	// run Serial commands
	while (Serial.available()) {
#ifdef DEBUG
		Gbl::strPtr->println(F("Serial.available"));
		Gbl::freeRam();
#endif
		Gbl::strPtr = &Serial;
		masterCtr.serialReceive();
		Gbl::freeRam();
	}
#ifdef MEGA
	while (Serial3.available()) {
        Gbl::strPtr = &Serial3;
        masterCtr.serialReceive();
        Gbl::freeRam();
    }
#endif

#ifdef BTUNO
	while (BT.available()) {
#ifdef DEBUG
		Gbl::strPtr->println(F("BT.available"));
		Gbl::freeRam();
#endif
        Gbl::strPtr = &BT;
        masterCtr.serialReceive();
        Gbl::freeRam();
    }
#endif

#ifdef IR
    if (irrecv.decode(&Results)) {
		masterCtr.irReceive(Results.value);
		irrecv.resume(); // Receive the next value
	}
#endif
}
