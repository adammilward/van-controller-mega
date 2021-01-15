#include "Storage.h"
#include "Gbl.h"

Storage::Storage() {
    uint8_t s = numSignals;
    while(s--){
        uint8_t i = numRecords;
        while (i--) {
            oneMin.records[s][i] =
            fiveMins.records[s][i] =
            thirtyMins.records[s][i] = 0;
        }
    }
}

void Storage::makeRecord(
    uint32_t timestamp,
    float values[],
    uint8_t valCount
) {
    // every minute
    addRecord(oneMin, timestamp, values, valCount);
    // 5 minute denominator
    if (! (storeCount % 5)) {
        if (storeCount)
            values = averageRecords(oneMin, timestamp, 5, values);
        addRecord(fiveMins, timestamp, values, valCount);
    }
    // thirty minute denominator
    if (! (storeCount % 30)) {
        if (storeCount)
            // Serial.print("values before averageing [");
            // Serial.print(values[0]);
            // Serial.print(", ");
            // Serial.print(values[1]);
            // Serial.print(", ");
            // Serial.print(values[2]);
            // Serial.println("]");
            values = averageRecords(oneMin, timestamp, 30, values);
            // Serial.print("values after averageing [");
            // Serial.print(values[0]);
            // Serial.print(", ");
            // Serial.print(values[1]);
            // Serial.print(", ");
            // Serial.print(values[2]);
            // Serial.println("]");
        addRecord(thirtyMins, timestamp, values, valCount); 
    }

    // increment counter
    storeCount ++;
    //reset counter once largest denominator reached
    if (storeCount == 30) {
        storeCount = 0;
    }
}

void Storage::addRecord(
    analogRecord &recordRef,
    uint32_t timestamp,
    float values[],
    uint8_t valCount
) {
    recordRef.timestamp = timestamp;

    while(valCount--){
        uint8_t i = numRecords;
        // copy elements from 58 - 0 into 59 - 1
        while (--i) {
            recordRef.records[valCount][i]
            =
            recordRef.records[valCount][i - 1];
        }
        // place the most recent value into last place
        recordRef.records[valCount][0] = values[valCount];
    }
}

float* Storage::averageRecords(
    analogRecord &source,
    uint32_t timestamp,
    uint8_t denominator,
    float* values
) {
    // build the array of averages
    uint8_t c = numSignals;
    while(c--) values[c] = 0;

    uint8_t i = numSignals;
    //Serial.println("averaging");
    while (i--) {
        // Serial.print("i = ");
        // Serial.println(i);
        uint8_t d = denominator;    
        while (d--) {
            // Serial.print("d = ");
            // Serial.println(d);
            values[i] += source.records[i][d];
            // Serial.println(values[i]);
        }
        values[i] = values[i] / denominator;
        // Serial.print("value[i] = ");
        // Serial.println(values[i]);
    }
    
    return values;
}

void Storage::output() {
    Gbl::strPtr->println(F("<{'type': 'records', 'payload': {'records': {"));
    Gbl::strPtr->print(F("'oneMin': "));
    outputRecords(oneMin);
    Gbl::strPtr->print(F(",'fiveMins': "));
    outputRecords(fiveMins);
    Gbl::strPtr->print(F(",'thirtyMins': "));
    outputRecords(thirtyMins);
    Gbl::strPtr->print(F("}}}>"));
}

void Storage::outputRecords(analogRecord &recordRef) {
    Gbl::strPtr->print(F("{'timestamp': "));
    Gbl::strPtr->println(recordRef.timestamp);
    
    Gbl::strPtr->print(F(",'values': ["));

    for(uint8_t s = 0; s < (numSignals); s++) {
        
        if (s) Gbl::strPtr->print(F(","));
        Gbl::strPtr->println(F("["));

        for(uint8_t i = 0; i < (numRecords); i++) {
            if (i) Gbl::strPtr->print(F(","));   
            Gbl::sprintf(recordRef.records[s][i]);
        }

        Gbl::strPtr->println(F("]"));
    }   
    
    Gbl::strPtr->println(F("]}"));
}

