#ifndef STEPPER_H
#define STEPPER_H

#include <hal/gpio_types.h>
// #define CONFIG_IDF_TARGET_ESP32S2
// #define SUPPORT_SELECT_DRIVER_TYPE
//#define SUPPORT_ESP32_RMT
#include "RMTStepper.h"
#include <memory>

namespace StepperDriver {

    enum Level {
            GPIO_LOW,
            GPIO_HIGH
    };

    enum Direction {
        CW,
        CCW
    };

    class StepperDirection {
        public:

        StepperDirection();

        StepperDirection(Direction aDir, Level aLevel);

        // Overloading the == operator
        bool operator==(const StepperDirection &other) const;
        // Overloading the != operator
        bool operator!=(const StepperDirection &other) const;

        // operator Level();

        operator uint32_t();

        // Overloading the == operator
        bool operator==(const Level &other) const;

        // Overloading the != operator
        bool operator!=(const Level &other) const;

        private:
            Level level;
            Direction dir;
    };

    class Stepper {
    public:
        const char *TAG = "Stepper";
        long init;

        enum DistancePerTimeUnit {
            IPM,
            MMPM
        };

        Stepper(
            //std::shared_ptr<FastAccelStepperEngine> engine,
            gpio_num_t enPin, 
            gpio_num_t dirPin, 
            gpio_num_t stepPin, 
            StepperDriver::Level enableLevel, 
            uint32_t motorResolutionHz, 
            StepperDriver::StepperDirection startupMotorDirection
        );

        void SetSpeed(uint32_t aDistancePerTime, DistancePerTimeUnit aUnit);
        void SetMaxSpeed(uint16_t aMaxSpeed);
        void SetFullSpeedAcceleration(uint16_t anAccelStepsPerSecond,uint16_t aDecelStepsPerSecond);

        void MoveDistance(uint32_t distance);
        void Run(Direction aDir);
        void Stop();

    private:
        StepperDirection defaultMotorDirection;
        // gpio_config_t en_dir_gpio_config;
        // FastAccelStepperEngine engine;
        // FastAccelStepper *stepper;
        RMTStepper* rmtStepper;
        uint32_t PrivIPMToHz(uint32_t aIPM);
        uint32_t PrivMMPMToHz(uint32_t aMMPM);
};
}

#endif