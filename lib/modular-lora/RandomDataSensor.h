#ifndef RANDOMDATASENSOR_H
#define RANDOMDATASENSOR_H

#include "Sensor.h"
#include "DataStrctures.h"

class RandomDataSensor : public Sensor<MinMaxAvg<int>> {
    public:
        RandomDataSensor(int rangeMin, int rangeMax);
        void measure();
        void reset();
    private:
        int rMin;
        int rMax;
        int count;
};


#endif