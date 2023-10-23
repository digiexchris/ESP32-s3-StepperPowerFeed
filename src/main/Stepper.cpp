#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "RmtStepper.h"
#include "Stepper.h"
namespace StepperDriver {
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
    ESP_LOGI(TAG, "Initialize EN + DIR GPIO");
    en_dir_gpio_config = {
        .pin_bit_mask = 1ULL << dirPin | 1ULL << enPin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&en_dir_gpio_config));

    ESP_LOGI(TAG, "Create RMT TX channel");
    motor_chan = NULL;
    tx_chan_config = {
        .gpio_num = stepPin,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select clock source
        .resolution_hz = motorResolutionHz,
        .mem_block_symbols = 64,
        .trans_queue_depth = 10, // set the number of transactions that can be pending in the background
        .flags = {}
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &motor_chan));

    ESP_LOGI(TAG, "Set spin direction");
    gpio_set_level(dirPin, defaultMotorDirection);
    ESP_LOGI(TAG, "Disable step motor");
    gpio_set_level(stepPin, !enableLevel);

    ESP_LOGI(TAG, "Create motor encoders");
    accel_encoder_config = {
        .resolution = motorResolutionHz,
        .sample_points = 500,
        .start_freq_hz = 500,
        .end_freq_hz = 1500,
    };
    accel_motor_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&accel_encoder_config, &accel_motor_encoder));

    uniform_encoder_config = {
        .resolution = motorResolutionHz,
    };
    uniform_motor_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_uniform_encoder(&uniform_encoder_config, &uniform_motor_encoder));

    decel_encoder_config = {
        .resolution = motorResolutionHz,
        .sample_points = 500,
        .start_freq_hz = 1500,
        .end_freq_hz = 500,
    };
    decel_motor_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&decel_encoder_config, &decel_motor_encoder));

    ESP_LOGI(TAG, "Enable RMT channel");
    ESP_ERROR_CHECK(rmt_enable(motor_chan));

    //rmt_transmit config
    tx_config = {
        .loop_count = 0,
        .flags = {}
    };
}

} //namespace StepperDriver