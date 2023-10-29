
#include <stdint.h>
#include <cmath>
#include "Exception.h"
#include "MovePlanner.h"

bool MovePlanner::PrivCanAccelerate(uint16_t aCurrentSpeed, uint16_t aTargetSpeed, uint16_t anAccelRate) {
    if(anAccelRate == 0) {
        return false;
    }
    if(aTargetSpeed <= aCurrentSpeed) {
        return false;
    }

    return true;
}

bool MovePlanner::PrivCanDecelerate(uint16_t aCurrentSpeed, uint16_t aTargetSpeed, uint16_t aDecellRate) {
    if (aDecellRate == 0) {
        return false;
    }
    if (aTargetSpeed >= aCurrentSpeed) {
        return false;
    }

    return true;
}

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
    
    uint32_t fullSpeedAccelSamples = 0;  
    if (PrivCanAccelerate(aCurrentSpeed, aTargetSpeed,anAccelRate)) {
        fullSpeedAccelSamples = CalculateSamples(aCurrentSpeed, aTargetSpeed, anAccelRate);
    }

    uint32_t fullSpeedDecellSamples = 0;
    if (aCurrentSpeed > aTargetSpeed) {
        fullSpeedDecellSamples = CalculateSamples(aCurrentSpeed, aTargetSpeed, anAccelRate);
    }

    uint32_t accSamples = 0;
    uint32_t decSamples = 0;

    //compensate if we can't reach the target speed in the number of steps available
    if(fullSpeedAccelSamples >= aStepsAvailable || fullSpeedDecellSamples == aStepsAvailable) {

        //what's the max speed we can reach?
        
        //determine if there are more acceleration or deceleration samples
        //redistribute samples
        //make sure accSamples + decSamples == aStepsToMove
        throw NotImplementedException("Not Implemented: accSamples == aStepsToMove || decSamples == uniformSamples");
    } else if (fullSpeedAccelSamples + fullSpeedDecellSamples == aStepsAvailable) {
        anOutUniformSamples = 0;
        //uniform samples is 0, but otherwise we're good to go
    } else if (aStepsAvailable - (fullSpeedAccelSamples + fullSpeedDecellSamples) < 0) {
        //there aren't enough samples left to decelerate. Redistribute.
        throw NotImplementedException("Not Implemented: uniformSamples - accSamples - decSamples < 0");
    } else if (aStepsAvailable - (fullSpeedAccelSamples + fullSpeedDecellSamples) > 0) {
        //the normal case
        anOutUniformSamples = aStepsAvailable - (fullSpeedAccelSamples + fullSpeedDecellSamples);
    } else {
        throw InvalidAccelerationCalculationException("Could not find a case to match TargetSpeed %s, Acc Rate %s, Dec Rate %s, in %d steps.");
    }

    anOutDecelSamples = decSamples;
    anOutAccelSamples = accSamples;
};

uint16_t MovePlanner::CalculateSamples(
    uint32_t anInitialSpeed, 
    uint32_t aTargetSpeed, 
    uint32_t aRate
) {
    /**
     * using   steps = (v^2 - u^2) / 2a 
     * where v = target speed, u = initial speed
     * a = acceleration in steps/second squared
     * 
    */

    return static_cast<uint16_t>(round(abs((pow(aTargetSpeed, 2) - pow(anInitialSpeed, 2)) / (aRate * 2))));
};