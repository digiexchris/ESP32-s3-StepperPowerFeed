#pragma once
//#include "stdint.h"
#include "Exception.h"

class MovePlanner {
    public:

    class InvalidAccelerationCalculationException : GenericException {
        public:
            using GenericException::GenericException;
    };

    /**
     * @brief calculate the number of samples required to go from the current speed
     * to the target speed, using the stated acceleration
     * 
     * @param uint32_t anInitialSpeed, 
       param uint32_t aTargetSpeed, 
       param uint32_t aRate
    */
    static uint16_t CalculateSamples(
        uint32_t anInitialSpeed, 
        uint32_t aTargetSpeed, 
        uint32_t aRate
        );

private:

};