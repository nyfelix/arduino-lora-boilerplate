#ifndef VOLTAGESENSOR_H
#define VOLTAGESENSOR_H

#include "Sensor.h"
#include "DataStrctures.h"

class VoltageSensor : public Sensor<MinMaxAvg> {
    public:
        VoltageSensor(int pinVoltage, float referenceVoltage);
        void measure();
        void reset();
    private:
        int pinVoltage;
        float referenceVoltage;
        int count;
};

#endif