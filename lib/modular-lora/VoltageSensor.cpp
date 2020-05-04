#include "VoltageSensor.h"
#include "Arduino.h"
#include <stdlib.h>

VoltageSensor::VoltageSensor(int pinVoltage, float referenceVoltage) :  pinVoltage{pinVoltage}, referenceVoltage{referenceVoltage} {

};

void VoltageSensor::measure() {
    float val = analogRead(pinVoltage);
    val *= 2;    // we divided by 2, so multiply back
    val *= referenceVoltage;  // Multiply by 3.3V, our reference voltage
    val /= 1024; // convert to voltage
    value.max = (val < value.max)? value.max : val;
    value.min = (val > value.max)? value.max : val;
    value.avg = (count * value.avg + val)/(count + 1);
    count++;
};

void VoltageSensor::reset() {
    value.min = 0;
    value.max = 0;
    value.avg = 0;
    count = 0;
}
