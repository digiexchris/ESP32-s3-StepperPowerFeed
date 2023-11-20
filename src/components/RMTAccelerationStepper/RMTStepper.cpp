#include "driver/gpio.h"
#include "esp_log.h"
#include "StepperMotorEncoder.h"
#include "RMTStepper.h"
#include <task.hpp>
// #include <cmath>
// #include <exception>
// #include <memory>
#include <condition_variable>
#include "RMTStepperStateMachineStates.hpp"
#include <StateMachine.hpp>
//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

const char* RMTStepper::TAG = "RMTStepper";

void RMTStepper::HandleEvent(const SM::Event& anEvent) {
    // if(myTransitionMap->find(typeid(anEvent)) == myTransitionMap->end()) {
    //     ESP_LOGE(TAG, "Event not found in transition map");
    //     return;
    // }
    // if(myTransitionMap->at(typeid(anEvent)).find(typeid(*myCurrentState)) == myTransitionMap->at(typeid(anEvent)).end()) {
    //     ESP_LOGE(TAG, "Event not valid for current state");
    //     return;
    // }
    // myTransitionMap->at(typeid(anEvent)).at(typeid(*myCurrentState))(*myCurrentState, anEvent);
}

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
        // myTargetPosition(0),
        // myCurrentPosition(0),
        myTargetSpeed(0),
        myCurrentSpeed(0),
        // myAccelerationRate(0),
        // myDecelerationRate(0),
        myDirection(0),
        myMaxStepperFreq(aMaxStepperFreq),
        myResolution(aResolution),
    
        myUniformQueue(0)
{
    //auto transitionMap = std::make_unique<StateMachine::TransitionMap>();
    myTransitionMap = std::make_unique<SM::TransitionMap>();
    myCurrentState = std::make_unique<StoppedState>();

    ESP_LOGI(TAG, "Initialize EN + DIR GPIO");
    const gpio_config_t en_dir_gpio_config = {
        .pin_bit_mask = 1ULL << myDirPin | 1ULL << myEnPin,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    ESP_ERROR_CHECK(gpio_config(&en_dir_gpio_config));

    ESP_LOGI(TAG, "Set spin direction");
    gpio_set_level(myDirPin, myCWDir);
    ESP_LOGI(TAG, "Disable step motor");
    gpio_set_level(myEnPin, !myEnableLevel);
    myIsEnabled = false;

    myRmt = std::make_unique<espp::Rmt>(espp::Rmt::Config{
        .gpio_num = aStepPin,
        .resolution_hz = aResolution,
        .log_level = espp::Logger::Verbosity::INFO,
    });

    //This config gets deep copied so it will remain accessable for future reuse when we want to make
    //a new uniform encoder.
    myUniformEncoderConfig = std::make_unique<espp::RmtEncoder::Config>(espp::RmtEncoder::Config{
        .bytes_encoder_config = {
            .bit0 =
                {
                    // divide the rmt transmit resolution (200hz) to get about 1us
                    .duration0 = static_cast<unsigned short>(aResolution / aMaxStepperFreq / 2),
                    //.level0 = 0,
                    //.duration1 = static_cast<unsigned short>(aResolution / aMaxStepperFreq / 2),
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

    auto aUniformEncoder = std::make_unique<espp::RmtEncoder>(*myUniformEncoderConfig);
        
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
    myRmt->set_encoder(std::move(aUniformEncoder));

    QueueMove(myUniformQueue, INT8_MAX, myUniformQueueMutex, myUniformQueueCV);
    
    myUniformEncoderTask = espp::Task::make_unique(
        {
            .name = "RMTSendUniformQueueStepsTask",
            .callback = [&]() -> bool {
                return SendQueuedSteps(myUniformQueue,myUniformQueueMutex,myUniformQueueCV);
            },
            .stack_size_bytes = 4096,
            .priority = 1,
            .log_level = espp::Logger::Verbosity::WARN,
        }
    );

    myUniformEncoderTask->start();

    
}


/**
 * @brief send all queued steps, one step at a time, and waiting before hand to allow
 * another thread to insert more steps into the queue, or to cancel the queue.
 * at rest, this task is not running.
 * 
 * RMTStepper == Uniform type, no acceleration.
 * @return true if the queue is empty, false if the queue is not empty.
*/
bool RMTStepper::SendQueuedSteps(uint8_t* aQueue, std::mutex& aQueueMutex, std::condition_variable& aQueueCV) {
    if(*aQueue > 0) {
        const uint8_t data[] = {1};

        std::unique_lock<std::mutex> lk(aQueueMutex);
        std::cv_status status = aQueueCV.wait_for(lk, static_cast<std::chrono::microseconds>(myResolution / myMaxStepperFreq)/2);

        if(status == std::cv_status::timeout) {
            ESP_LOGW(TAG, "SendUniformQueuedSteps timeout");
        }
        else if(status == std::cv_status::no_timeout) {
            ESP_LOGI(TAG, "SendUniformQueuedSteps no_timeout, shutting down");
            return true;
        }
        else {
            ESP_LOGI(TAG, "SendUniformQueuedSteps unknown");
        }

        //assume we're going to send this pulse. if we error in the transmit we'll maybe have to add it back.
        //this is so the queue can be returned for more queuing while we transmit.
        --(*aQueue);
        lk.unlock();
        aQueueCV.notify_one(); 

        if(!myRmt->transmit(data, sizeof(data))){
            //todo add the step back if this is a recurring problem and we actually want to handle it
            ESP_LOGE(TAG, "SendUniformQueuedSteps transmit error, may or may not have lost a step");
            return false;
        };

        //there's still more in the queue, , maybe keep going
        //probably transition to decelerating here. maybe fire a "UniformQueueEmpty event."
        //The UniformQueueEmptyEvent should probably switch the primary queue to the deceleration queue, 
        //and set the myRmt to the DecelEncoder.
        return *aQueue == 0;
    }
    return true;
}
      
void RMTStepper::SetDirection(uint8_t aDirection) {
        //Fire a direction change event, which will fire a direction change error event if it's running
        // myDirection = aDirection;
        // ESP_LOGI(TAG, "Set spin direction");
        // gpio_set_level(myDirPin, myDirection);
    }

size_t RMTStepper::QueueMove(uint8_t* aQueue, uint8_t aStepsToMove, std::mutex& aQueueMutex, std::condition_variable& aQueueCV) {

    //TODO this is assuming NO acceleration at the moment. It will naievely queue up all steps as uniform steps.
    //TODO make this calculate the number of acceleration steps required based on current speed, and the number of
    //deceleration steps required at the end.

    std::unique_lock<std::mutex> lk(aQueueMutex);
    aQueueCV.wait(lk);

    uint16_t sum = static_cast<uint16_t>(*aQueue) + static_cast<uint16_t>(aStepsToMove);
    uint8_t overflow = 0;

    if (sum > INT8_MAX) {
        overflow = sum - UINT8_MAX;
        *aQueue = UINT8_MAX;
    }

    lk.unlock();
    aQueueCV.notify_one();

    if(overflow) {
        while(overflow > 0) {
            overflow = QueueMove(aQueue, overflow, aQueueMutex, aQueueCV);
        }
    }
    return overflow;
}