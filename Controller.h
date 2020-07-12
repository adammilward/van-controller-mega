/*
 * RemoteReceive.h
 *
 *  Created on: 20 Feb 2017
 *      Author: Adam Milward
 */

#include "LightCtr.h"
#include "TimeCtr.h"
#include "StatusCtr.h"

#ifndef REMOTERECEIVE_H_
#define REMOTERECEIVE_H_

class Controller {
public:

	Controller();
    const unsigned long int IR_HOLD = 0xFFFFFFFF;
    void irReceive(unsigned long value);
    void serialReceive();
    void timer(unsigned long);

    static bool isNum(char* wordAr);

    LightCtr lightCtr;
    TimeCtr timeCtr;
    StatusCtr statusCtr;

private:

    enum IRMode {IR_LIGHTS, IR_MP3};
    IRMode iRMode = IR_LIGHTS;
    enum Mode {LIGHTS, STATUS, CLOCK};
    Mode mode = CLOCK;

    unsigned long int storedCode = 0;  // for sending when hold is pressed

    static constexpr byte maxLetters = 30;
    char dataArr[maxLetters];
    byte dataArrLength = 0;

    static constexpr byte maxWords = 7;
    char *wordPtrs[maxWords];
    byte wordCount = 0;

    bool serialGetCommand();
    void addChar(char);
    void addSpace();
    void terminateCommand();

    bool checkForRepeat(char *);
    bool processSerial(byte);
    bool checkForMode();
    void outputMode();


    void irDecode(unsigned long, int);
};
#endif /* REMOTERECEIVE_H_ */
