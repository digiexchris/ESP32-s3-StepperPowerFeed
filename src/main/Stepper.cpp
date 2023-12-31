// #include "driver/rmt_tx.h"
#include <driver/gpio.h>
#include "esp_log.h"
#include "RMTStepper.h"
#include "Stepper.h"
#include <exception>
#include <memory>

namespace StepperDriver {

class InvalidStepperException : std::exception {
    public:
        InvalidStepperException(char* aMsg) {
            msg = aMsg;
            delete(aMsg);
        }
        const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return msg;
        };
        private:
            char* msg;
};
class InvalidDistancePerTimeUnitException : std::exception {

};

StepperDirection::StepperDirection() {
    level = Level::GPIO_LOW;
    dir = Direction::CW;
}

StepperDirection::StepperDirection(Direction aDir, Level aLevel) {
    level = aLevel;
    dir = aDir;
}

// Overloading the == operator
bool StepperDirection::operator==(const StepperDirection &other) const {
    return level == other.level && dir == other.dir;
}

// Overloading the != operator
bool StepperDirection::operator!=(const StepperDirection &other) const {
    return !(*this == other); // Reusing the == operator
}

// StepperDirection::operator Level() {
//     return level;
// }

StepperDirection::operator uint32_t() {
    return static_cast<uint32_t>(level);
}

// Overloading the == operator
bool StepperDirection::operator==(const Level &other) const {
    return static_cast<int>(level) == other;
}

// Overloading the != operator
bool StepperDirection::operator!=(const Level &other) const {
    return !(*this == other); // Reusing the == operator
}

bool StepperDirection:: operator==(const Direction& other) const {
    return static_cast<int>(dir) == other;
};

bool StepperDirection::operator!=(const Direction& other) const {
    return !(*this == other);
};

void Stepper::MoveDistance(uint32_t distance) {
    //rmtStepper->Move(1000);
}

/**
 * @brief set motor acceleration/deceleration
 * Note: this is linear acceleration, from zero to full speed and back to zero.
 * @param uint32_t steps #the number of steps to accelerate up to speed. 
 * A closed loop stepper may be able to set this to 0 if all moves are slow enough.
 * A servo may not need any acceleration for any moves, the closed loop driver
 * may handle it.
*/
void Stepper::SetFullSpeedAcceleration(uint16_t anAccelStepsPerSecond,uint16_t aDecelStepsPerSecond){
    // stepper->setLinearAcceleration(steps);
}

// void Stepper::SetDirection(Direction aDir) {
//     //todo not needed, run forward is fine.
// }

void Stepper::Run(Direction aDir) {
    if(aDir == Direction::CW) {
        //stepper->runForward();
    } else {
        //stepper->runBackward();
    }
    
}

void Stepper::Stop() {
    //stepper->stopMove();
}

/**
 * @brief Set the continuous speed that the stepper will ramp up to in distance per unit of time, eg inches per minute
 * NOTE: This will execute in mm per minute and all other units will convert to it.
*/
void Stepper::SetSpeed(uint32_t aDistancePerTime, DistancePerTimeUnit aUnit) {
    switch(aUnit) {
        case DistancePerTimeUnit::IPM:
        //stepper->setSpeedInUs(1000);
            //stepper->setSpeedInHz(PrivIPMToHz(aDistancePerTime));
            break;
        case DistancePerTimeUnit::MMPM:
            //stepper->setSpeedInHz(PrivMMPMToHz(aDistancePerTime));
            break;
        default:
            throw InvalidDistancePerTimeUnitException();
            return;
    }

    //if it's running continuously, set the new speed immediately
    //useful for rapids or changing IPM during a cut.
    //stepper->applySpeedAcceleration();
}

/**
 * @brief convert inches per minute to hz
*/
uint32_t Stepper::PrivIPMToHz(uint32_t aIPM) {
    return (aIPM * 25.4 * 60);
}

/**
 * @brief convert mm per minute to hz
*/
uint32_t Stepper::PrivMMPMToHz(uint32_t aMMPM) {
    return (aMMPM * 60);
}


//NOTE pullups disabled. Use appropriate resistors.
Stepper::Stepper(
    //std::shared_ptr<FastAccelStepperEngine> engine,
    gpio_num_t enPin, 
    gpio_num_t dirPin,
    gpio_num_t stepPin, 
    StepperDriver::Level enableLevel, //Is enable high or low on your driver?
    uint16_t rmtResolutionHz, 
    uint16_t aMaxStepperFreq,
    StepperDriver::StepperDirection startupMotorDirection // default motor direction and level. inverting this reverses direction (and by extension, level of the dir pin)
    ) : defaultMotorDirection(startupMotorDirection)
{
    rmtStepper = new RMTStepper(
        stepPin, 
        dirPin, 
        enPin, 
        startupMotorDirection, 
        enableLevel,
        rmtResolutionHz,
        aMaxStepperFreq
    );

}

} //namespace StepperDriver