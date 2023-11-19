
#include <cstdint>
#include <cmath>
#include "Exception.h"
#include "MovePlanner.h"

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