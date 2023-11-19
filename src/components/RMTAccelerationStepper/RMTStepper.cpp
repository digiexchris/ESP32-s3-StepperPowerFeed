#include "driver/gpio.h"
#include "esp_log.h"
#include "StepperMotorEncoder.h"
#include "RMTStepper.h"
#include <task.hpp>
#include <cmath>
#include <exception>
#include <memory>
#include <condition_variable>
//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

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
        uint32_t aResolution = 10000000,
        uint32_t aMaxStepperFreq = 200000
    ) : myStepPin(aStepPin),
        myDirPin(aDirPin),
        myEnPin(anEnPin),
        myCWDir(aCWDir),
        myCCWDir(!aCWDir),
        myEnableLevel(anEnableLevel),
        myTargetPosition(0),
        myCurrentPosition(0),
        myTargetSpeed(0),
        myCurrentSpeed(0),
        myAccelerationRate(0),
        myDecelerationRate(0),
        myDirection(0),
        myMaxStepperFreq(aMaxStepperFreq),
        myResolution(aResolution),
    
        myUniformQueue(0)
{
    ESP_LOGI(TAG, "Initialize EN + DIR GPIO");
    gpio_config_t en_dir_gpio_config = {
        .pin_bit_mask = 1ULL << myDirPin | 1ULL << myEnPin,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    ESP_ERROR_CHECK(gpio_config(&en_dir_gpio_config));

    // ESP_LOGI(TAG, "Create RMT TX channel");
    // myMotorChan = NULL;
    // myTxChanConfig = {
    //     .gpio_num = myStepPin,
    //     .clk_src = RMT_CLK_SRC_DEFAULT, // select clock source
    //     .resolution_hz = myResolution,
    //     .mem_block_symbols = 64,
    //     .trans_queue_depth = 10, // set the number of transactions that can be pending in the background
    // };
    // ESP_ERROR_CHECK(rmt_new_tx_channel(&myTxChanConfig, &myMotorChan));

    ESP_LOGI(TAG, "Set spin direction");
    gpio_set_level(myDirPin, myCWDir);
    ESP_LOGI(TAG, "Disable step motor");
    gpio_set_level(myEnPin, !myEnableLevel);
    myIsEnabled = false;

    // ESP_LOGI(TAG, "Create motor encoders");
    // myAccelEncoderConfig = {
    //     .resolution = myResolution,
    //     .sample_points = 500,
    //     .start_freq_hz = 500,
    //     .end_freq_hz = 1500,
    // };
    // myAccelEncoder = NULL;
    // ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&myAccelEncoderConfig, &myAccelEncoder));

    // myUniformEncoderConfig = {
    //     .resolution = myResolution,
    // };
    // myUniformEncoder = NULL;
    // ESP_ERROR_CHECK(rmt_new_stepper_motor_uniform_encoder(&myUniformEncoderConfig, &myUniformEncoder));

    // myDecelEncoderConfig = {
    //     .resolution = myResolution,
    //     .sample_points = 500,
    //     .start_freq_hz = 1500,
    //     .end_freq_hz = 500,
    // };
    // myDecelEncoder = NULL;
    // ESP_ERROR_CHECK(rmt_new_stepper_motor_curve_encoder(&myDecelEncoderConfig, &myDecelEncoder));

    // ESP_LOGI(TAG, "Enable RMT channel");
    // ESP_ERROR_CHECK(rmt_enable(myMotorChan));

    // ESP_LOGI(TAG, "Spin motor for 6000 steps: 500 accel + 5000 uniform + 500 decel");
    // myTxConfig = {
    //     .loop_count = 0,
    // };

    

    myRmt = std::make_unique<espp::Rmt>(espp::Rmt::Config{
        .gpio_num = aStepPin,
        .resolution_hz = aResolution,
        .log_level = espp::Logger::Verbosity::INFO,
    });

    auto uniform_encoder = std::make_unique<espp::RmtEncoder>(espp::RmtEncoder::Config{
        .bytes_encoder_config = {
            .bit0 =
                {
                    // divide the rmt transmit resolution (200hz) to get about 1us
                    .duration0 = static_cast<unsigned short>(myResolution / myMaxStepperFreq / 2),
                    //.level0 = 0,
                    //.duration1 = static_cast<unsigned short>(myResolution / myMaxStepperFreq / 2),
                    // .level1 = 1,
                },
            .flags =
                {
                    .msb_first = 1 // WS2812 transfer bit order: G7...G0R7...R0B7...B0
                },
        },
        .encode = UniformEncoder::EncodeFn,
        .del = UniformEncoder::DelFn,
        .reset = UniformEncoder::ResetFn
    });

    std::mutex m;
    std::condition_variable cv;

    myRmt->set_encoder(std::move(uniform_encoder));

    espp::Task uniformQueueTask = espp::Task(espp::Task::SimpleConfig{
        .name = "RMTSendUniformQueueStepsTask",
        .callback = [&]() -> bool {
            return SendUniformQueuedSteps(myUniformQueueMutex, myUniformQueueCV);
        },
        .stack_size_bytes = 4096,
        .priority = 1,
        .log_level = espp::Logger::Verbosity::WARN,
    });
}


/**
 * @brief send all queued steps, one step at a time, and waiting before hand to allow
 * another thread to insert more steps into the queue, or to cancel the queue.
*/
bool RMTStepper::SendUniformQueuedSteps(std::mutex &m, std::condition_variable &cv) {
    
    std::unique_lock<std::mutex> lk(m);
    if(myUniformQueue > 0) {
        
        //wait for the time between pulses (maybe)
        std::cv_status status = cv.wait_for(lk, static_cast<std::chrono::microseconds>(myResolution / myMaxStepperFreq / 2));

        //if notimeout, it means we're shutting down, so cancel all this.
        //eg if we've immediately switched from uniform to decel phase while
        //there is still steps in the queue (stop the power feed, etc)
        switch(status) {
            case std::cv_status::no_timeout:
                myUniformQueue = 0;
                return true;
            default:
                break;
        }

        const uint8_t data[] = {1};

        myRmt->transmit(data, sizeof(data));
        myUniformQueue--;
        if(myUniformQueue > 0) {
            return false;
        }
    }

    return true;
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
    
    // const static uint32_t accel_samples = 500;
    // const static uint32_t uniform_speed_hz = 1500; //eg steps per second
    // const static uint32_t decel_samples = 500;

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
        // ESP_ERROR_CHECK(rmt_transmit(myMotorChan, myAccelEncoder, &accel_samples, sizeof(accel_samples), &myTxConfig));

        // // uniform phase
        // myTxConfig.loop_count = 5000;
        // ESP_ERROR_CHECK(rmt_transmit(myMotorChan, myUniformEncoder, &uniform_speed_hz, sizeof(uniform_speed_hz), &myTxConfig));

        // // deceleration phase
        // myTxConfig.loop_count = 0;
        // ESP_ERROR_CHECK(rmt_transmit(myMotorChan, myDecelEncoder, &decel_samples, sizeof(decel_samples), &myTxConfig));
        // // wait all transactions finished
        // ESP_ERROR_CHECK(rmt_tx_wait_all_done(myMotorChan, -1));

        ESP_LOGI(TAG, "Disable step motor");
        gpio_set_level(myEnPin, !myEnableLevel);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Accelerate(#);
    // MoveUniformSteps(#);
    // Decelerate(#);
}