#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "stepper_motor_encoder.h"
#include "RMTStepper.h"
#include <cmath>
#include <exception>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

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
    myMotorChan = NULL;
    myTxChanConfig = {
        .gpio_num = myStepPin,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select clock source
        .resolution_hz = myResolution,
        .mem_block_symbols = 64,
        .trans_queue_depth = 10, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&myTxChanConfig, &myMotorChan));

    ESP_LOGI(TAG, "Set spin direction");
    gpio_set_level(myDirPin, myCWDir);
    ESP_LOGI(TAG, "Disable step motor");
    gpio_set_level(myEnPin, !myEnableLevel);
    myIsEnabled = false;

    ESP_LOGI(TAG, "Create motor encoders");
    myAccelEncoderConfig = {
        .resolution = myResolution,
        .sample_points = 500,
        .start_freq_hz = 500,
        .end_freq_hz = 1500,
    };
    myAccelEncoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&myAccelEncoderConfig, &myAccelEncoder));

    myUniformEncoderConfig = {
        .resolution = myResolution,
    };
    myUniformEncoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_uniform_encoder(&myUniformEncoderConfig, &myUniformEncoder));

    myDecelEncoderConfig = {
        .resolution = myResolution,
        .sample_points = 500,
        .start_freq_hz = 1500,
        .end_freq_hz = 500,
    };
    myDecelEncoder = NULL;
    ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&myDecelEncoderConfig, &myDecelEncoder));

    ESP_LOGI(TAG, "Enable RMT channel");
    ESP_ERROR_CHECK(rmt_enable(myMotorChan));

    ESP_LOGI(TAG, "Spin motor for 6000 steps: 500 accel + 5000 uniform + 500 decel");
    myTxConfig = {
        .loop_count = 0,
    };

    
}

void RMTStepper::SetDirection(uint8_t aDirection) {
        //use the FSM to coordinate this properly. Shouldn't reverse if not stopped first.
        myDirection = aDirection;
        ESP_LOGI(TAG, "Set spin direction");
        gpio_set_level(myDirPin, myDirection);
    }

void RMTStepper::Move(uint64_t aStepsToMove) {
    //if this is < current position, use the FSM to reverse it.
    //calculate number of required samples for acceleration
    //currently is just from 0 speed to target and back to zero.
    //make it go to a new target speed instead of 0
    
    const static uint32_t accel_samples = 500;
    const static uint32_t uniform_speed_hz = 1500; //eg steps per second
    const static uint32_t decel_samples = 500;

    int i = 0;
    while (1) {
        if(i == 0) {
            i = 1;
        } else {
            i = 0;
        }
        ESP_LOGI(TAG, "Enable step motor");
        gpio_set_level(myEnPin, myEnableLevel);
        gpio_set_level(myDirPin, i);
        vTaskDelay(pdMS_TO_TICKS(50));
        // acceleration phase
        myTxConfig.loop_count = 0;
        ESP_ERROR_CHECK(rmt_transmit(myMotorChan, myAccelEncoder, &accel_samples, sizeof(accel_samples), &myTxConfig));

        // uniform phase
        myTxConfig.loop_count = 5000;
        ESP_ERROR_CHECK(rmt_transmit(myMotorChan, myUniformEncoder, &uniform_speed_hz, sizeof(uniform_speed_hz), &myTxConfig));

        // deceleration phase
        myTxConfig.loop_count = 0;
        ESP_ERROR_CHECK(rmt_transmit(myMotorChan, myDecelEncoder, &decel_samples, sizeof(decel_samples), &myTxConfig));
        // wait all transactions finished
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(myMotorChan, -1));

        ESP_LOGI(TAG, "Disable step motor");
        gpio_set_level(myEnPin, !myEnableLevel);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Accelerate(#);
    // MoveUniformSteps(#);
    // Decelerate(#);
}