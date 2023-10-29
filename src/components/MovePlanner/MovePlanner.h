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

    /// @brief Generate the 3 sets of stepping parameters used for linear accel/decel
    /// @param anOutAccelSamples 
    /// @param anOutDecelSamples 
    /// @param anOutUniformSteps 
    /// @param aTargetSpeed 
    /// @param aCurrentSpeed 
    /// @param anAccelRate
    /// @param aDecelRate 
    /// @param aStepsAvailable The total number of steps in the move
    static void GenerateMove(
        uint32_t& anOutAccelSamples, 
        uint32_t& anOutDecelSamples, 
        uint32_t& anOutUniformSteps,
        uint16_t aTargetSpeed,
        uint16_t aCurrentSpeed,
        uint16_t anAccelRate,
        uint16_t aDecelRate,
        uint16_t aStepsAvailable
    );

private:
    static bool PrivCanAccelerate(uint16_t aCurrentSpeed, uint16_t aTargetSpeed, uint16_t anAccelRate);

    static bool PrivCanDecelerate(uint16_t aCurrentSpeed, uint16_t aTargetSpeed, uint16_t aDecellRate);

};