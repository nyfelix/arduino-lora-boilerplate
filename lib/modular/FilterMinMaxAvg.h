#ifndef FILTERMINMAXAVG_H
#define FILTERMINMAXAVG_H

#include "Filter.h"

template <typename T> // Datatype to filter 

class FilterMinMaxAvg : public Filter {
    public:
        void run(T* input, MinMaxAvg<T>* output){           
            output->max = (input > output->max || count == 0)? input : output->max;
            output->min = (input < output->min || count == 0)? input : output->min;
            output->avg = (count * output->avg + input)/(count + 1);
            count++;
        };
        void reset() {
            count = 0;
        }
    
    protected:
        int count {0};
};

#endif