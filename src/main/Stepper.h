
#include "driver/gpio.h"
// #define CONFIG_IDF_TARGET_ESP32S2
#define SUPPORT_SELECT_DRIVER_TYPE
//#define SUPPORT_ESP32_RMT
#include <FastAccelStepper.h>

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

        Stepper(
            gpio_num_t enPin, 
            gpio_num_t dirPin, 
            gpio_num_t stepPin, 
            StepperDriver::Level enableLevel, 
            uint32_t motorResolutionHz, 
            StepperDriver::StepperDirection startupMotorDirection
        );

    private:
        StepperDirection defaultMotorDirection;
        gpio_config_t en_dir_gpio_config;
        FastAccelStepperEngine engine;
        FastAccelStepper *stepper;
        

        // rmt_channel_handle_t motor_chan;
        // rmt_tx_channel_config_t tx_chan_config;
        // stepper_motor_curve_encoder_config_t accel_encoder_config;
        // rmt_encoder_handle_t accel_motor_encoder;
        // stepper_motor_uniform_encoder_config_t uniform_encoder_config;
        // rmt_encoder_handle_t uniform_motor_encoder;
        // stepper_motor_curve_encoder_config_t decel_encoder_config;
        // rmt_encoder_handle_t decel_motor_encoder;
        // rmt_transmit_config_t tx_config;
};
}

