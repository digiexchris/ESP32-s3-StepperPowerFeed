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
#include "StepperMotorEncoder.h"

//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
class RMTStepper  {
public:
    RMTStepper(
        gpio_num_t aStepPin, 
        gpio_num_t aDirPin, 
        gpio_num_t anEnPin, 
        uint8_t aCWDir, 
        uint8_t anEnableLevel,
        uint32_t aResolution,
        uint32_t aMaxStepperFreq
    );
    static const char *TAG;

    void SetTargetSpeed(
        uint16_t aSpeed, 
        uint16_t anAccelerationRate, 
        uint16_t aDecelerationRate) {
        myTargetSpeed = aSpeed;
        myAccelerationRate = anAccelerationRate;
        myDecelerationRate = aDecelerationRate;
        // myFullSpeedAccelerationSteps = MovePlanner::CalculateSamples(0, myTargetSpeed, myAccelerationRate);
        // myFullSpeedDecellerationSteps = MovePlanner::CalculateSamples(0, myTargetSpeed, myDecelerationRate);
    }

    void SetDirection(uint8_t aDirection);

    void Move(uint64_t aStepsToMove);

    //keep adding to the queue as the queue gets consumed I guess?
    void Run();

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

    
    /**
     * @brief accelerate to the target speed across the number of steps passed
     * @param uint32_t inSteps // The number of steps to spread the acceleration over
    */
    void Accelerate(uint32_t inSteps, uint16_t toTargetSpeed);

    /**
     * @brief execute the passed number of steps at the current speed
     * @param uint32_t numSteps // number of steps to execute
    */
    void MoveUniformSteps(uint32_t numSteps);

    /**
     * @brief accelerate to the target speed across the number of steps passed
     * @param uint32_t inSteps // The number of steps to spread the acceleration over
    */
    void Decelerate(uint32_t inSteps, uint16_t toTargetSpeed);

    gpio_num_t myStepPin;
    gpio_num_t myDirPin;
    gpio_num_t myEnPin;
    uint8_t myCWDir;
    uint8_t myCCWDir;
    uint8_t myEnableLevel;

    uint64_t myTargetPosition;
    uint64_t myCurrentPosition;
    uint16_t myTargetSpeed; //steps per second
    uint16_t myCurrentSpeed;
    uint16_t myAccelerationRate;

    uint16_t myDecelerationRate;
    uint8_t myDirection;
    
    uint16_t myMaxStepperFreq; // Reordered member variable
    uint16_t myResolution; // Reordered member variable
    bool myIsEnabled;
    uint32_t myFullSpeedAccelerationSteps; //Stores how many steps it takes to go from 0 to the current target speed
    uint32_t myFullSpeedDecelerationSteps; //Stores how many steps it takes to go from the current target speed to a full stop
    std::unique_ptr<espp::Rmt> myRmt;
    rmt_channel_handle_t myMotorChan;
    rmt_tx_channel_config_t myTxChanConfig;
    std::unique_ptr<espp::RmtEncoder::Config> myAccelEncoderConfig;
    //std::unique_ptr<CurveEncoder> myAccelEncoder; // Corrected spelling if needed
    std::unique_ptr<espp::RmtEncoder::Config> myUniformEncoderConfig;
    std::unique_ptr<UniformEncoder> myUniformEncoder; // Fixed template argument
    std::unique_ptr<espp::RmtEncoder::Config> myDecelEncoderConfig;
    //std::unique_ptr<CurveEncoder> myDecelEncoder;
    rmt_transmit_config_t myTxConfig;

    uint8_t myUniformQueue;
    std::mutex myUniformQueueMutex;
    std::condition_variable myUniformQueueCV;
    bool SendUniformQueuedSteps(std::mutex &m, std::condition_variable &cv);
    //todo calculate these based off of speed and accel per time unit
    uint32_t myAccelSamples;
    uint32_t myUniformSpeedHz;
    uint32_t myDecelSamples;

};