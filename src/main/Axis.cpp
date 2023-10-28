#include "Axis.h"
#include <memory>

Axis::Axis(
    char aLabel, 
    //std::shared_ptr<FastAccelStepperEngine> engine,
    gpio_num_t enPin, 
    gpio_num_t dirPin, 
    gpio_num_t stepPin, 
    StepperDriver::Level enableLevel, 
    uint32_t motorResolutionHz, 
    StepperDriver::StepperDirection startupMotorDirection
) {
    label = aLabel;
    stepper = new StepperDriver::Stepper(
        //engine,
        enPin, 
        dirPin, 
        stepPin, 
        enableLevel, 
        motorResolutionHz, 
        startupMotorDirection
    );
}