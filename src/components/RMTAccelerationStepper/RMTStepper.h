#pragma once
#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include "StepperMotorEncoder.h"
#include "driver/rmt_tx.h"
#include "RMTStepper.h"
#include <string>
#include <exception>
#include <esp_log.h>
#include "Exception.h"
#include "Moveplanner.h"
#include <memory>
#include <rmt.hpp>
#include <rmt_encoder.hpp>
#include <condition_variable>
#include <StateMachine.hpp>
#include <task.hpp>

//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
class RMTStepper : public SM::StateMachine {
public:
    enum class QueueMode {
        FIFO = 0,
        CONTINUOUS
    };

    RMTStepper(
        gpio_num_t aStepPin, 
        gpio_num_t aDirPin, 
        gpio_num_t anEnPin, 
        uint8_t aCWDir, 
        uint8_t anEnableLevel = 1, 
        uint32_t aResolution = 10000000,
        uint32_t aMaxStepperFreq = 200000
    );
    static const char *TAG;

    void SetTargetSpeed(
        uint16_t aSpeed
        // uint16_t anAccelerationRate, 
        // uint16_t aDecelerationRate
    ) {
        myTargetSpeed = aSpeed;
        // myAccelerationRate = anAccelerationRate;
        // myDecelerationRate = aDecelerationRate;
        // myFullSpeedAccelerationSteps = MovePlanner::CalculateSamples(0, myTargetSpeed, myAccelerationRate);
        // myFullSpeedDecellerationSteps = MovePlanner::CalculateSamples(0, myTargetSpeed, myDecelerationRate);
    }

    void SetDirection(uint8_t aDirection);

    void HandleEvent(const SM::Event& anEvent) override;

    void SetMode(QueueMode aMode) {
        // if(myState != RMTStepperState::STOPPED) {
        //     ESP_LOGE(TAG, "Cannot change queue mode while motor is running");
        //     return;
        // }
        myMode = aMode;
    }

    /**
     * @brief Queue aStepsToMove number of pulses
     * @param int8_t aStepsToMove // The number of steps to move.
    */
    size_t QueueMove(uint8_t* aQueue, uint8_t aStepsToMove, std::mutex& aQueueMutex, std::condition_variable& aQueueCV);

    //keep adding to the queue as the queue gets consumed I guess?
    void MoveContinuous(uint8_t aDirection);

    //empty the uniform queue and decelerate using the FSM
    void Stop();

    void Enable() {
        ESP_LOGI(TAG, "Enable step motor");
        gpio_set_level(myEnPin, myEnableLevel);
        myIsEnabled = true;
    }

    void Disable() {
        ESP_LOGI(TAG, "Disable step motor");
        gpio_set_level(myEnPin, !myEnableLevel);
        myIsEnabled = false;
    }

private:

    
    // /**
    //  * @brief accelerate to the target speed across the number of steps passed
    //  * @param uint32_t inSteps // The number of steps to spread the acceleration over
    // */
    // void Accelerate(uint32_t inSteps, uint16_t toTargetSpeed);

    /**
     * @brief execute the passed number of steps at the current speed
     * @param uint32_t numSteps // number of steps to execute
    */
    void MoveUniformSteps(uint32_t numSteps);

    // /**
    //  * @brief accelerate to the target speed across the number of steps passed
    //  * @param uint32_t inSteps // The number of steps to spread the acceleration over
    // */
    // void Decelerate(uint32_t inSteps, uint16_t toTargetSpeed);

    gpio_num_t myStepPin;
    gpio_num_t myDirPin;
    gpio_num_t myEnPin;
    uint8_t myCWDir;
    uint8_t myCCWDir;
    uint8_t myEnableLevel;

    // uint64_t myTargetPosition;
    // uint64_t myCurrentPosition;
    uint16_t myTargetSpeed; //steps per second
    uint16_t myCurrentSpeed;
    // uint16_t myAccelerationRate;

    // uint16_t myDecelerationRate;
    uint8_t myDirection;
    
    uint16_t myMaxStepperFreq; // Reordered member variable
    uint16_t myResolution; // Reordered member variable
    bool myIsEnabled;

    /**
     * @brief The queue mode to use when moving
     * FIFO = the queue consumes steps as it transmits them and stops
     * CONTINUOUS = the queue ignored by the transmitter unless the queue is empty.
    */
    QueueMode myMode;

    // uint32_t myFullSpeedAccelerationSteps; //Stores how many steps it takes to go from 0 to the current target speed
    // uint32_t myFullSpeedDecelerationSteps; //Stores how many steps it takes to go from the current target speed to a full stop
    std::unique_ptr<espp::Rmt> myRmt;
    // rmt_channel_handle_t myMotorChan;
    // rmt_tx_channel_config_t myTxChanConfig;
    //std::unique_ptr<espp::RmtEncoder::Config> myAccelEncoderConfig;
    //std::unique_ptr<CurveEncoder> myAccelEncoder; // Corrected spelling if needed
    //std::unique_ptr<espp::RmtEncoder::Config> myUniformEncoderConfig;
    std::unique_ptr<espp::RmtEncoder> myUniformEncoder;
    std::unique_ptr<espp::RmtEncoder::Config> myUniformEncoderConfig;
    std::unique_ptr<espp::Task> myUniformEncoderTask;
    //std::unique_ptr<espp::RmtEncoder::Config> myDecelEncoderConfig;
    //std::unique_ptr<CurveEncoder> myDecelEncoder;
    //rmt_transmit_config_t myTxConfig;

    uint8_t* myUniformQueue;
    std::mutex myUniformQueueMutex;
    std::condition_variable myUniformQueueCV;
    bool SendQueuedSteps(uint8_t* aQueue, std::mutex& aQueueMutex, std::condition_variable& aQueueCV);
    //todo calculate these based off of speed and accel per time unit
    //uint32_t myAccelSamples;
    //uint32_t myUniformSpeedHz;
    //uint32_t myDecelSamples;

};