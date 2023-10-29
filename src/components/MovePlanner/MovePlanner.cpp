
#include <stdint.h>
#include <cmath>
#include "../Exception/Exception.h"
#include "MovePlanner.h"
void MovePlanner::GenerateMove(
    uint32_t& anOutAccelSamples, 
    uint32_t& anOutDecelSamples, 
    uint32_t& anOutUniformSamples,
    uint16_t aTargetSpeed,
    uint16_t aCurrentSpeed,
    uint16_t anAccelRate,
    uint16_t aDecelRate,
    uint16_t aStepsAvailable
) {
    uint32_t accSamples = CalculateSamples(aCurrentSpeed, aTargetSpeed, anAccelRate, aStepsAvailable);
    uint32_t decSamples = CalculateSamples(aTargetSpeed,aCurrentSpeed, aDecelRate, aStepsAvailable);
    uint32_t uniformSamples = aStepsAvailable;
    //compensate if we can't reach the target speed in the number of steps available
    if(accSamples == aStepsAvailable || decSamples == uniformSamples) {
        //determine if there are more acceleration or deceleration samples
        //redistribute samples
        //make sure accSamples + decSamples == aStepsToMove
        throw NotImplementedException("Not Implemented: accSamples == aStepsToMove || decSamples == uniformSamples");
    } else if (accSamples + decSamples == uniformSamples) {
        throw NotImplementedException("Not Implemented: accSamples + decSamples == uniformSamples");
        //uniform samples is 0, but otherwise we're good to go
    } else if (uniformSamples - accSamples - decSamples < 0) {
        //there aren't enough samples left to decelerate. Redistribute.
        throw NotImplementedException("Not Implemented: uniformSamples - accSamples - decSamples < 0");
    } else if (uniformSamples - accSamples - decSamples > 0) {
        //the normal case
        uniformSamples = uniformSamples - accSamples - decSamples;
    } else {
        throw InvalidAccelerationCalculationException("Could not find a case to match TargetSpeed %s, Acc Rate %s, Dec Rate %s, in %d steps.");
    }

    anOutUniformSamples = uniformSamples;
    anOutDecelSamples = decSamples;
    anOutAccelSamples = accSamples;
};

uint16_t MovePlanner::CalculateSamples(
    uint32_t anInitialSpeed, 
    uint32_t aTargetSpeed, 
    uint32_t aRate, 
    uint32_t aTotalAvailableSteps
) {
    /**
     * using   steps = (v^2 - u^2) / 2a 
     * where v = target speed, u = initial speed
     * a = acceleration in steps/second squared
     * 
    */

   return (pow(aTargetSpeed, 2) - pow(anInitialSpeed, 2)) / pow(aRate,2);
};