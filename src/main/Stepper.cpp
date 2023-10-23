// #include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "FastAccelStepper.h"
#include "Stepper.h"
#include <exception>

namespace StepperDriver {

    class InvalidStepperException : std::exception {
        public:
            InvalidStepperException(char* aMsg) {
                msg = aMsg;
                delete(aMsg);
            }
            virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
                return msg;
            };
            private:
                char* msg;
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

    StepperDirection::operator Level() {
        return level;
    }

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

///////////////////////////////Change the following configurations according to your board//////////////////////////////
// #define STEP_MOTOR_GPIO_EN       0
// #define STEP_MOTOR_GPIO_DIR      2
// #define STEP_MOTOR_GPIO_STEP     4
// #define STEP_MOTOR_ENABLE_LEVEL  0 // DRV8825 is enabled on low level
// #define STEP_MOTOR_SPIN_DIR_CLOCKWISE 0
// #define STEP_MOTOR_SPIN_DIR_COUNTERCLOCKWISE !STEP_MOTOR_SPIN_DIR_CLOCKWISE

// #define STEP_MOTOR_RESOLUTION_HZ 1000000 // 1MHz resolution

//NOTE pullups disabled. Use appropriate resistors.
Stepper::Stepper(
    gpio_num_t enPin, 
    gpio_num_t dirPin,
    gpio_num_t stepPin, 
    StepperDriver::Level enableLevel, //Is enable high or low on your driver?
    uint32_t motorResolutionHz, 
    StepperDriver::StepperDirection startupMotorDirection // default motor direction and level. inverting this reverses direction (and by extension, level of the dir pin)
    ) : defaultMotorDirection(startupMotorDirection)
{

//     #define dirPinStepper 18
// #define enablePinStepper 26
// #define stepPinStepper 17

    engine = FastAccelStepperEngine();
    stepper = NULL;

    stepper = engine.stepperConnectToPin(stepPin);
    
    if(!stepper) {
        throw InvalidStepperException("Could not connect to stepper pin");
    }

    stepper->setDirectionPin(dirPin);
    stepper->setEnablePin(enPin);
    stepper->setAutoEnable(true);

    // If auto enable/disable need delays, just add (one or both):
    // stepper->setDelayToEnable(50);
    // stepper->setDelayToDisable(1000);

    // speed up in ~0.025s, which needs 625 steps without linear mode
    stepper->setSpeedInHz(50000);
    stepper->setAcceleration(2000000);

}

} //namespace StepperDriver