#include "RandomDataSensor.h"
#include <stdlib.h>

RandomDataSensor::RandomDataSensor(int rangeMin, int rangeMax) :  rMin{rangeMin}, rMax{rangeMax} {
    reset();
};

void RandomDataSensor::measure() {
    int val = rand() % (rMax-rMin) + rMin; 
    value.max = (val > value.max || count == 0)? val : value.max;
    value.min = (val < value.min || count == 0)? val : value.min;
    value.avg = (count * value.avg + val)/(count + 1);
    count++;
};

void RandomDataSensor::reset() {
    value.min = 0;
    value.max = 0;
    value.avg = 0;
    count = 0;
}
