#ifndef RANDOMDATASENSOR_H
#define RANDOMDATASENSOR_H

#include "Sensor.h"
#include "DataStrctures.h"

template <typename T>

class RandomDataSensor : public Sensor {
    public:
        RandomDataSensor(T rangeMin, T rangeMax) : Sensor(Filter filter), rMin{rangeMin}, rMax{rangeMax} {
            reset();
        };
        void measure() {
            T val = rMin + rand()*(rMax-rMin);
        };

        void reset() {
            //value.min = 0;
            //value.max = 0;
            //value.avg = 0;
            count = 0;
        };
    private:
        T rMin;
        T rMax;
        int count {0};
};


#endif