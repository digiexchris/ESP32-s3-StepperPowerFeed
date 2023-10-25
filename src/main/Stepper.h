#ifndef STEPPER_H
#define STEPPER_H

#include <hal/gpio_types.h>
// #define CONFIG_IDF_TARGET_ESP32S2
#define SUPPORT_SELECT_DRIVER_TYPE
//#define SUPPORT_ESP32_RMT
#include <FastAccelStepper.h>
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

        operator Level();

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

        enum DistancePerTimeUnit {
            IPM,
            MMPM
        };

        Stepper(
            std::shared_ptr<FastAccelStepperEngine> engine,
            gpio_num_t enPin, 
            gpio_num_t dirPin, 
            gpio_num_t stepPin, 
            StepperDriver::Level enableLevel, 
            uint32_t motorResolutionHz, 
            StepperDriver::StepperDirection startupMotorDirection
        );

        void SetSpeed(uint32_t aDistancePerTime, DistancePerTimeUnit aUnit);
        void SetAcceleration(uint32_t anAcceleration);
        //don't need to set direction, move and run handle direction
        //void SetDirection(StepperDriver::Direction aDir);
        void MoveDistance();
        void Run(Direction aDir);
        void Stop();

    private:
        StepperDirection defaultMotorDirection;
        gpio_config_t en_dir_gpio_config;
        FastAccelStepperEngine engine;
        FastAccelStepper *stepper;
        
        uint32_t PrivIPMToHz(uint32_t aIPM);
        uint32_t PrivMMPMToHz(uint32_t aMMPM);
};
}

#endif