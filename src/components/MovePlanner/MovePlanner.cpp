
#include <stdint.h>
#include <cmath>
#include "Exception.h"
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
    //I don't like this pattern, figure out a different way of planning out the move sequences.
    // Such as, if we have a queue of uniform steps still, and it's more than CalculateSamples(currentspeed,0, decel) away from the end of the queue
    // then we don't need to calculate deceleration. 
    // if we are, start adding in decel samples.
    // if we get another move command, stop queuing up decel samples and start queieing up accel samples until the queue is at a specific length
    // or until we've reached uniform speed? maybe?
    // 
    // actually start with implementing the FSM first, so we can get a stateful flow first. this is a power feed, not a cnc controller.
    // heck, scrap this all and just make it start, run, and stop on command.
    // 
    // 
    // 
    //compensate if we can't reach the target speed in the number of steps available
    if(fullSpeedAccelSamples >= aStepsAvailable || fullSpeedDecellSamples == aStepsAvailable) {

        //what's the max speed we can reach?
        
        //determine if there are more acceleration or deceleration samples
        //redistribute samples
        //make sure accSamples + decSamples == aStepsToMove
        throw NotImplementedException("Not Implemented: accSamples == aStepsToMove || decSamples == uniformSamples");
    } else if (aStepsAvailable - (fullSpeedAccelSamples + fullSpeedDecellSamples) < 0) {
        //there aren't enough samples left to decelerate. Redistribute.
        throw NotImplementedException("Not Implemented: uniformSamples - accSamples - decSamples < 0");
    } else if (aStepsAvailable - (fullSpeedAccelSamples + fullSpeedDecellSamples) >= 0) {
        //the normal case
        anOutUniformSamples = aStepsAvailable - (fullSpeedAccelSamples + fullSpeedDecellSamples);
        anOutAccelSamples = fullSpeedAccelSamples;
        anOutDecelSamples = fullSpeedDecellSamples;
    } else {
        throw InvalidAccelerationCalculationException("Could not find a case to match TargetSpeed %s, Acc Rate %s, Dec Rate %s, in %d steps.");
    }
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