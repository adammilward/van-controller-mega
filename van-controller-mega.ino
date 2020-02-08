#define DEBUG

#include "Arduino.h"
#include "Controller.h"
#include "Gbl.h"
#include "LightCtr.h"



Controller masterCtr;      // handles the remotes
Stream *Gbl::strPtr = &Serial;

Light Red(Gbl::RED_PIN, 0);
Light Green(Gbl::GREEN_PIN, 1);
Light Blue(Gbl::BLUE_PIN, 2);
Light* LightCtr::red = &Red;
Light* LightCtr::green = &Green;
Light* LightCtr::blue = &Blue;

//#define MEGA
//#define BTUNO
#ifdef BTUNO
	#include "SoftwareSerial.h"
	SoftwareSerial BT = SoftwareSerial(12, 13); // RX,TX
#endif

#define PI_UNO
#ifdef PI_UNO
	#include "SoftwareSerial.h"
	SoftwareSerial piSerial = SoftwareSerial(2, 3); // RX,TX
#endif

//#define TME
#ifdef TME
#include <DS3231.h>
//#include "TimeCtr.h"
DS3231  Clock(SDA, SCL);
DS3231* TimeCtr::clock = &Clock;
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
#ifdef PI_UNO
	piSerial.begin(115200); // RX,TX
#endif
#ifdef TME
	Clock.begin();
#endif
	Gbl::strPtr->println(F("we are programmed to receive"));
#ifdef MEGA
	Gbl::strPtr = &Serial3;
	Gbl::strPtr->println(F("we are programmed to receive"));
#endif
#ifdef BTUNO
	Gbl::strPtr = &BT;
	Gbl::strPtr->println(F("we are programmed to receive"));
#endif
#ifdef piSerial
	Gbl::strPtr = &piSerial;
	Gbl::strPtr->println(F("we are programmed to receive"));
#endif
	LightCtr::setFadeOffQuick(1);
}


void loop()
{
	// for anything running on a timer
	masterCtr.timer(millis());

	// run Serial commands
	while (Serial.available()) {
		Gbl::strPtr = &Serial;
		masterCtr.serialReceive();
	}

#ifdef MEGA
	while (Serial3.available()) {
        Gbl::strPtr = &Serial3;
        masterCtr.serialReceive();
    }
#endif

#ifdef BTUNO
	while (BT.available()) {
        Gbl::strPtr = &BT;
        masterCtr.serialReceive();
    }
#endif

#ifdef PI_UNO
	while (piSerial.available()) {
        Gbl::strPtr = &piSerial;
        masterCtr.serialReceive();
    }
#endif
}
