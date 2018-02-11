/*
 * Config.h
 *
 *  Created on: 11 Mar 2017
 *      Author: Adam Milward
 */

#include "Arduino.h"

#define UNO
//#define MEGA

#ifndef CONFIG_H_
#define CONFIG_H_
class Gbl {
public:

	static Stream *strPtr;

    static const byte BT_RX = 12;
    static const byte BT_TX = 13;

    static const byte DELAY_MIN = 1;
    static const unsigned int DELAY_MAX = 256;
#ifdef MEGA		                      //   ________
    static const byte RED_PIN = 4;    //  |   __   |
    static const byte GREEN_PIN = 3;  //  |  /  \  |
    static const byte BLUE_PIN = 2;   //  | |    | |
    static const byte IREC_PIN = 5;  //  out  0  v+
#endif
#ifdef UNO		                      //   ________
    static const byte RED_PIN = 9;    //  |   __   |
    static const byte GREEN_PIN = 10;  //  |  /  \  |
    static const byte BLUE_PIN = 11;   //  | |    | |
    static const byte IREC_PIN = 5;  //  out  0  v+
#endif


    static constexpr float A0_FACTOR = 0.027305029;
    static constexpr float A1_FACTOR = 0.015564788;
    static constexpr float A2_FACTOR = 0.015415081;
    static constexpr float A3_FACTOR = 0.015522867;
    static constexpr float A4_FACTOR = 0.01540874;

/*    static constexpr float A0_FACTOR = 0.02583;
    static constexpr float A1_FACTOR = 0.01473;
    static constexpr float A2_FACTOR = 0.01461;
    static constexpr float A3_FACTOR = 0.01467;
    static constexpr float A4_FACTOR = 0.01460;*/

    static void freeRam();

};
#endif /* CONFIG_H_ */
