#include "driver/gpio.h"
#include "esp_log.h"
#include "StepperMotorEncoder.h"
#include "RMTStepper.h"
#include <task.hpp>
// #include <cmath>
// #include <exception>
// #include <memory>
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
        uint8_t anEnableLevel, 
        uint32_t aResolution,
        uint32_t aMaxStepperFreq
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
    const gpio_config_t en_dir_gpio_config = {
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
//todo I THINK this is how we start the move
/**
 * 1: set the encoder to accel_encoder
 * 2: queue up accel moves
 * 3: start the accel move task
 * 4: wait for the accel move task to finish
 * 5: set the encoder to uniform_encoder
 * 6: queue up uniform moves
 * 7: start the uniform move task
 * 8: wait for the uniform move task to finish
 * 9: set the encoder to decel_encoder
 * 10: queue up decel moves
 * 11: start the decel move task
 * 12: wait for the decel move task to finish
 * 
 * In any of the queue moves logic, if QueueMove returns a non-zero value, 
 * then we need to queue up the remaining steps in a loop
 * after we start the task. We probably need to check if the queue is empty when
 * we first wait on the task, because if it's empty then we don't need to wait. 
 * Also if it wasn't empty, but got sent before we got to the wait, don't need
 * to wait either.
 * 
 * So, this shouldn't be in the constructor, but in a Move() or MoveContinuous() function.
 * 
 * Stop can simply delete the encoder (which should stop the task, and reset the queue via the Encoder::DelFn)
*/
    myRmt->set_encoder(std::move(uniform_encoder));

    espp::Task uniformQueueTask = espp::Task(espp::Task::SimpleConfig{
        .name = "RMTSendUniformQueueStepsTask",
        .callback = [&]() -> bool {
            return SendUniformQueuedSteps();
        },
        .stack_size_bytes = 4096,
        .priority = 1,
        .log_level = espp::Logger::Verbosity::WARN,
    });
}


/**
 * @brief send all queued steps, one step at a time, and waiting before hand to allow
 * another thread to insert more steps into the queue, or to cancel the queue.
 * at rest, this task is not running.
 * @return true if the queue is empty, false if the queue is not empty.
*/
bool RMTStepper::SendUniformQueuedSteps() {
    if(myUniformQueue > 0 || myUniformQueue < 0) {
        std::unique_lock<std::mutex> lk(myUniformQueueMutex);
        myUniformQueueCV.wait(lk, static_cast<std::chrono::microseconds>(myResolution / myMaxStepperFreq));
        const uint8_t data[] = {1};

        myRmt->transmit(data, sizeof(data));
        if(myUniformQueue > 0) {
            myUniformQueue--;
        }
        else {
            myUniformQueue++;
        }
        lk.unlock();
        myUniformQueueCV.notify_one(); 

        if(myUniformQueue > 0 || myUniformQueue < 0) {
            return false;
        }

        return true;
    }

    return true;
}
      
//todo probably not required.
void RMTStepper::SetDirection(uint8_t aDirection) {
        //use the FSM to coordinate this properly. Shouldn't reverse if not stopped first.
        //probably not required because queuemove allows negative moves.
        myDirection = aDirection;
        ESP_LOGI(TAG, "Set spin direction");
        gpio_set_level(myDirPin, myDirection);
    }

size_t RMTStepper::QueueMove(int8_t aStepsToMove) {

    //TODO this is assuming NO acceleration at the moment. It will naievely queue up all steps as uniform steps.
    //TODO make this calculate the number of acceleration steps required based on current speed, and the number of
    //deceleration steps required at the end.

    std::unique_lock<std::mutex> lk(myUniformQueueMutex);
    myUniformQueueCV.wait(lk, static_cast<std::chrono::microseconds>(myResolution / myMaxStepperFreq));
    
    auto m = std::lock_guard(myUniformQueueMutex);

    int16_t sum = myUniformQueue + aStepsToMove;
    int8_t overflow = 0;

    if (sum > INT8_MAX) {
        overflow = sum - INT8_MAX;
        myUniformQueue = INT8_MAX;
    } else if (sum < INT8_MIN) {
        overflow = sum - INT8_MIN;
        myUniformQueue = INT8_MIN;
    }

    lk.unlock();
    myUniformQueueCV.notify_one();
    return overflow;
}