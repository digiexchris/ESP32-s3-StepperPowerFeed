
#include "driver/rmt_tx.h"
#include "RmtStepper.h"

#define STEPPER_CW true
#define STEPPER_CCW !STEPPER_CW

#define LEVEL_LOW false
#define LEVEL_HIGH !LEVEL_LOW

namespace StepperDriver {

    enum Level {
            LOW = 0,
            HIGH = 1
    };

    enum Direction {
        CW,
        CCW
    };

    class StepperDirection {
        public:

        StepperDirection() {
            level = LOW;

        }
        
        StepperDirection(Direction aDir, Level aLevel) {
            level = aLevel;
            dir = aDir;
        }

        // Overloading the == operator
        bool operator==(const StepperDirection &other) const {
            return level == other.level && dir == other.dir;
        }

        // Overloading the != operator
        bool operator!=(const StepperDirection &other) const {
            return !(*this == other); // Reusing the == operator
        }

        operator Level() {
            return level;
        }

        operator uint32_t() {
            return static_cast<uint32_t>(level);
        }

        // Overloading the == operator
        bool operator==(const Level &other) const {
            return static_cast<int>(level) == other;
        }

        // Overloading the != operator
        bool operator!=(const Level &other) const {
            return !(*this == other); // Reusing the == operator
        }

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
        rmt_channel_handle_t motor_chan;
        rmt_tx_channel_config_t tx_chan_config;
        stepper_motor_curve_encoder_config_t accel_encoder_config;
        rmt_encoder_handle_t accel_motor_encoder;
        stepper_motor_uniform_encoder_config_t uniform_encoder_config;
        rmt_encoder_handle_t uniform_motor_encoder;
        stepper_motor_curve_encoder_config_t decel_encoder_config;
        rmt_encoder_handle_t decel_motor_encoder;
        rmt_transmit_config_t tx_config;
};
}

