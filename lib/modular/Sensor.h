#ifndef SENSOR_H
#define SENSOR_H

#include "Filter.h"

template <typename V> // Processed outputdata of Sensor

class Sensor {
    public:
        Sensor(Filter filter) : mfilter{filter};
        virtual void measure() {};
        V getValue() {
            return value;
        };

        virtual void reset() {

        };

    protected:
        V value;
        Filter mfilter;
};

#endif