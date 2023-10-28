#ifndef AXIS_H
#define AXIS_H

#include <memory>
#include "Stepper.h"

class Axis {
    public:
        Axis(
            char aLabel, 
            //std::shared_ptr<FastAccelStepperEngine> engine,
            gpio_num_t enPin, 
            gpio_num_t dirPin, 
            gpio_num_t stepPin, 
            StepperDriver::Level enableLevel, 
            uint32_t motorResolutionHz, 
            StepperDriver::StepperDirection startupMotorDirection
            );
        char label;
        StepperDriver::Stepper* stepper;
};

#endif