/*
 * LightCtr.h
 *
 *  Created on: 22 Feb 2017
 *      Author: Adam Milward
 */
#include "Utilities/Light.h"
#include "Gbl.h"
#include "Arduino.h"


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

    float colourStore[4][3] = {
            {0.2, -1, -1}, // red
            {0.5, -1, 0.2}, // purple
            {0.5, 0.2, -1}, // turcoise
            {0.5, 0.2, 0.2}, // dim white
    };
    float tempStore[3] = {0,0,0}; // for light power while holding


    void setReportDelay(float delay = 0);
    void retrieveStore(colour);
    void storeThis(colour);
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


    static constexpr byte oneWordCount = 8;
    static constexpr byte oneWordLength = 7;
    static const char oneWordCommands[oneWordCount][oneWordLength];

    PTR oneWordActions[oneWordCount]{
        &LightCtr::report,
        &LightCtr::allTop,
        &LightCtr::allBot,
        &LightCtr::turnFadingOff,
        &LightCtr::lin,
        &LightCtr::sin,
        &LightCtr::exp,
        &LightCtr::sinExp
    };

    const static byte firstOfTwoCount = 7;
    const static byte firstOfTwoLength = 6;
    static const char firstOfTwoCommands[firstOfTwoCount][firstOfTwoLength];

    const static byte secondOfTwoCount = 5;
    const static byte secondOfTwoLength = 7;
    static const char secondOfTwoCommands[secondOfTwoCount][secondOfTwoLength];

    const static byte wordAndFloatCount = 6;
    const static byte wordAndFloatLength = 7;
    static const char wordAndFloatCommands[wordAndFloatCount][wordAndFloatLength];

    PTR_f wordAndFloatActions[wordAndFloatLength] = {
            &LightCtr::setReportDelay,
            &LightCtr::allSet,
            &LightCtr::redSet,
            &LightCtr::greenSet,
            &LightCtr::blueSet,
            &LightCtr::delaySet,
    };

    PTR twoWordActions[firstOfTwoCount][secondOfTwoCount]{
{&LightCtr::allUp, &LightCtr::allDown, &LightCtr::allBot, &LightCtr::allTop, &LightCtr::off},// all
{&LightCtr::redUp, &LightCtr::redDown, &LightCtr::redBot, &LightCtr::redTop, &LightCtr::redOff},// red
{&LightCtr::greenUp, &LightCtr::greenDown, &LightCtr::greenBot, &LightCtr::greenTop, &LightCtr::greenOff},// green
{&LightCtr::blueUp, &LightCtr::blueDown, &LightCtr::blueBot, &LightCtr::blueTop, &LightCtr::blueOff},// blue
{&LightCtr::red_f, &LightCtr::orange_f, &LightCtr::lowerBot, &LightCtr::lowerTop, &LightCtr::null},// lower
{&LightCtr::green_f, &LightCtr::yellow_f, &LightCtr::upperBot, &LightCtr::upperTop, &LightCtr::null },// upper
{&LightCtr::white_f, &LightCtr::purple_f, &LightCtr::delayBot, &LightCtr::delayTop, &LightCtr::turnFadingOff }// fadeDelay
    };

    unsigned long int codes[20] = {
        0xFFA05F,   0xFF20DF,   0x000001,   0xFFE01F,
        0xFF906F,   0xFF10EF,   0xFF50AF,   0xFFD02F,
        0xFFB04F,   0xFF30CF,   0xFF708F,   0xFFF00F,
        0xFFA857,   0xFF28D7,   0xFF6897,   0xFFE817,
        0xFF9867,   0xFF18E7,   0xFF58A7,   0xFFD827
    };
    PTR actions[2][20] = {
        {   // first array for static mode
&LightCtr::allUp,    &LightCtr::allDown,   &LightCtr::on,   &LightCtr::off   ,
&LightCtr::redUp,   &LightCtr::greenUp,  &LightCtr::blueUp, &LightCtr::blueUp ,
&LightCtr::redDown,&LightCtr::greenDown, &LightCtr::blueDown, &LightCtr::blueDown,
&LightCtr::store1, &LightCtr::store2,  &LightCtr::store3,&LightCtr::store4 ,
 &LightCtr::lin,    &LightCtr::sin,     &LightCtr::exp,   &LightCtr::sinExp    ,
        },
        {   // second array for fade mode
&LightCtr::allUp,    &LightCtr::allDown,   &LightCtr::on,   &LightCtr::off   ,
&LightCtr::red_f,   &LightCtr::green_f,  &LightCtr::white_f, &LightCtr::white_f ,
&LightCtr::orange_f,&LightCtr::yellow_f, &LightCtr::purple_f, &LightCtr::purple_f,
&LightCtr::store1, &LightCtr::store2,  &LightCtr::store3,&LightCtr::store4 ,
&LightCtr::lin,    &LightCtr::sin,     &LightCtr::exp,   &LightCtr::sinExp    ,
        }
    };


void allUp();      void allDown();    void on();      void off();
void redUp();     void greenUp ();                  void blueUp ();
void redDown();  void greenDown();                  void blueDown();
void store1 ();  void store2 ();  void store3();  void store4 ();
void lin    ();  void sin    ();  void exp    ();  void sinExp    ();

void red_f();     void green_f ();                void white_f ();
void orange_f();  void yellow_f();                void purple_f();

void turnFadingOff();

void allSet(float);
void redSet(float);
void greenSet(float);
void blueSet(float);
void delaySet(float);

void allBot(); void allTop();
void redBot(); void redTop(); void redOff();
void greenBot(); void greenTop(); void greenOff();
void blueBot(); void blueTop(); void blueOff();
void lowerBot(); void lowerTop();
void upperBot(); void upperTop();
void delayBot(); void delayTop();

void null();

};
#endif /* LightCtr_H_ */
