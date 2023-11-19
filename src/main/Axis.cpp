#include "Axis.h"
#include <memory>

Axis::Axis(
    char aLabel, 
    //std::shared_ptr<FastAccelStepperEngine> engine,
    gpio_num_t enPin, 
    gpio_num_t dirPin, 
    gpio_num_t stepPin, 
    StepperDriver::Level enableLevel, 
    uint16_t stepperMaxFreq, 
    StepperDriver::StepperDirection startupMotorDirection,
    uint16_t rmtResolutionHz
) {
    label = aLabel;
    stepper = new StepperDriver::Stepper(
        //engine,
        enPin, 
        dirPin, 
        stepPin, 
        enableLevel, 
        rmtResolutionHz,
        stepperMaxFreq,
        startupMotorDirection
    );
}