#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "stepper_motor_encoder.h"
#include "RMTStepper.h"

const char* RMTStepper::TAG = "RMTStepper";

/**
 * Ideas:
 * use the RMT sync manager for multiple axis synchronization
 * I think the reset function is not implemented in stepper_motor_encoder.c, might be required for a stop command
*/

RMTStepper::RMTStepper(
        gpio_num_t aStepPin, 
        gpio_num_t aDirPin, 
        gpio_num_t anEnPin, 
        uint8_t aCWDir = 0,
        uint8_t anEnableLevel = 1, 
        uint32_t aResolution = 1000000
    ) : myStepPin(aStepPin),
        myDirPin(aDirPin),
        myEnPin(anEnPin),
        myCWDir(aCWDir),
        myCCWDir(!aCWDir),
        myEnableLevel(anEnableLevel),
        myResolution(aResolution),
        myTargetPosition(0),
        myCurrentPosition(0),
        myTargetSpeed(0),
        myCurrentSpeed(0),
        myAccelerationRate(0),
        myDecelerationRate(0),
        myDirection(0)
{
    ESP_LOGI(TAG, "Initialize EN + DIR GPIO");
    gpio_config_t en_dir_gpio_config = {
        .pin_bit_mask = 1ULL << myDirPin | 1ULL << myEnPin,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    ESP_ERROR_CHECK(gpio_config(&en_dir_gpio_config));

    ESP_LOGI(TAG, "Create RMT TX channel");
    rmt_channel_handle_t motor_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = myStepPin,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select clock source
        .resolution_hz = myResolution,
        .mem_block_symbols = 64,
        .trans_queue_depth = 10, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &motor_chan));

    ESP_LOGI(TAG, "Set spin direction");
    gpio_set_level(myDirPin, myCWDir);
    ESP_LOGI(TAG, "Disable step motor");
    gpio_set_level(myEnPin, !myEnableLevel);
    myIsEnabled = false;

    ESP_LOGI(TAG, "Create motor encoders");
    stepper_motor_curve_encoder_config_t accel_encoder_config = {
        .resolution = myResolution,
        .sample_points = 500,
        .start_freq_hz = 500,
        .end_freq_hz = 1500,
    };
    rmt_encoder_handle_t accel_motor_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&accel_encoder_config, &accel_motor_encoder));

    stepper_motor_uniform_encoder_config_t uniform_encoder_config = {
        .resolution = myResolution,
    };
    rmt_encoder_handle_t uniform_motor_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_uniform_encoder(&uniform_encoder_config, &uniform_motor_encoder));

    stepper_motor_curve_encoder_config_t decel_encoder_config = {
        .resolution = myResolution,
        .sample_points = 500,
        .start_freq_hz = 1500,
        .end_freq_hz = 500,
    };
    rmt_encoder_handle_t decel_motor_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&decel_encoder_config, &decel_motor_encoder));

    ESP_LOGI(TAG, "Enable RMT channel");
    ESP_ERROR_CHECK(rmt_enable(motor_chan));

    ESP_LOGI(TAG, "Spin motor for 6000 steps: 500 accel + 5000 uniform + 500 decel");
    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
    };

    const static uint32_t accel_samples = 500;
    const static uint32_t uniform_speed_hz = 1500; //eg steps per second
    const static uint32_t decel_samples = 500;

    while (1) {
        // acceleration phase
        tx_config.loop_count = 0;
        ESP_ERROR_CHECK(rmt_transmit(motor_chan, accel_motor_encoder, &accel_samples, sizeof(accel_samples), &tx_config));

        // uniform phase
        tx_config.loop_count = 5000;
        ESP_ERROR_CHECK(rmt_transmit(motor_chan, uniform_motor_encoder, &uniform_speed_hz, sizeof(uniform_speed_hz), &tx_config));

        // deceleration phase
        tx_config.loop_count = 0;
        ESP_ERROR_CHECK(rmt_transmit(motor_chan, decel_motor_encoder, &decel_samples, sizeof(decel_samples), &tx_config));
        // wait all transactions finished
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(motor_chan, -1));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
