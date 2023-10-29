#pragma once
#include "stdint.h"
#include "Exception.h"
class MovePlanner {
    public:

    class InvalidAccelerationCalculationException : GenericException {
        public:
            using GenericException::GenericException;
    };

    /**
     * @brief calculate the number of samples required to go from the current speed
     * to the target speed, if possible within the number of samples available
     * keeping it bound within the available acceleration/deceleration rate available.
     * eg. if we can accelerate 10s^2, and we only have 2 samples available, and we're starting at
     * 0 speed, and the target speed is 100, we'll only reach 20/sec speed. Number of samples is therefore 
     * capped at 2. NOTE: this is probably an error condition, since there are no moves available to
     * decelerate. detect that in the calling function.
     * 
     * If we can do the same acceleration, but we have 1500 steps available,
     * we will require 10 steps to get up to speed, so return 10. 
     * //todo, check that math,
     * it's late right now.
     * 
     * @param uint32_t anInitialSpeed, 
       param uint32_t aTargetSpeed, 
       param uint32_t aRate, 
       param uint32_t aTotalAvailableSteps
    */
    static uint16_t CalculateSamples(
        uint32_t anInitialSpeed, 
        uint32_t aTargetSpeed, 
        uint32_t aRate, 
        uint32_t aTotalAvailableSteps
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

};