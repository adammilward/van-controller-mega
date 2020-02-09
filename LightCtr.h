/*
 * LightCtr.h
 *
 *  Created on: 22 Feb 2017
 *      Author: Adam Milward
 */
#include <Arduino.h>
#include "Gbl.h"
#include "Light.h"

#ifndef LightCtr_H_
#define LightCtr_H_


// applies commands from lights remote control
// to instances of lights (Red Green and Blue)
class LightCtr {
public:
    LightCtr();

    static Light *red;
    static Light *green;
    static Light *blue;
    byte holdCount = 0;          // count how long button pressed

    enum controllerMode {STATIC, FADE};
    static controllerMode ctrMode;

    static uint16_t fadeDelay;
    static uint32_t waitMillisFade; // for timeing the next event.

    static uint8_t slideDelay;
    static uint32_t waitMillisSlide;

    byte reportDelaySec = 0;
    unsigned long waitMillisReport = 0;

    bool actionSerial(char **, byte);
    bool actionRemote(unsigned long);
    void interrupt();
    void onOffFade();
    void report();
    void timer(unsigned long);

    static void setFadeOffQuick(byte);
    static void setFadeSlow(byte, int8_t inShiftOp = 1);

private:

    enum colour { RED, GREEN, BLUE, WHITE };
    colour LightColour = colour::WHITE;

    void setReportDelay(float delay = 0);
    void turnSlideOn();

    void help();

    bool actionOneWord(char **);
    bool actionTwoWords(char **);
    bool parseTwoWords(char **);
    bool actionWordAndFloat(char **, float);


    static constexpr byte remoteAliasCount = 20;
    static constexpr byte remoteAliasLength = 7;
	static const char remoteAlias[remoteAliasCount][remoteAliasLength];

    // declare the function pointer
    typedef void (LightCtr::*PTR)();
    typedef void (LightCtr::*PTR_f)(float);


    static constexpr byte oneWordCount = 14;
    static constexpr byte oneWordLength = 7;
    static const char oneWordCommands[oneWordCount][oneWordLength];

    PTR oneWordActions[oneWordCount]{
        &LightCtr::stopReport,
        &LightCtr::allTop,
        &LightCtr::off,
        &LightCtr::turnFadingOff,
        &LightCtr::lin,
        &LightCtr::sin,
        &LightCtr::exp,
        &LightCtr::sinExp,
        &LightCtr::redUp,
        &LightCtr::redDown,
        &LightCtr::greenUp,
        &LightCtr::greenDown,
        &LightCtr::blueUp,
        &LightCtr::blueDown
    };

    const static byte firstOfTwoCount = 7;
    const static byte firstOfTwoLength = 6;
    static const char firstOfTwoCommands[firstOfTwoCount][firstOfTwoLength];

    const static byte wordAndFloatCount = 7;
    const static byte wordAndFloatLength = 7;
    static const char wordAndFloatCommands[wordAndFloatCount][wordAndFloatLength];

    PTR_f wordAndFloatActions[wordAndFloatLength] = {
            &LightCtr::setReportDelay,
            &LightCtr::redSet,
            &LightCtr::greenSet,
            &LightCtr::blueSet,
            &LightCtr::delaySet,
            &LightCtr::upperSet,
            &LightCtr::lowerSet,
    };

    void stopReport();
    void allTop();
    void off();
    void turnFadingOff();
    void exp();
    void sin();
    void lin();
    void sinExp();

    void redUp();  
    void greenUp ();
    void blueUp ();
    void redDown();
    void greenDown();
    void blueDown();

    void redSet(float);
    void greenSet(float);
    void blueSet(float);
    
    void delaySet(float);
    void upperSet(float);
    void lowerSet(float);

};
#endif /* LightCtr_H_ */
