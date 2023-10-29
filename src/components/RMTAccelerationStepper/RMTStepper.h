#pragma once
#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include "stepper_motor_encoder.h"
#include "driver/rmt_tx.h"
#include "RMTStepper.h"
#include <string>
#include <exception>
#include <esp_log.h>
#include "Exception.h"
#include "Moveplanner.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
class RMTStepper {
public:
    RMTStepper(
        gpio_num_t aStepPin, 
        gpio_num_t aDirPin, 
        gpio_num_t anEnPin, 
        uint8_t aCWDir, 
        uint8_t anEnableLevel,
        uint32_t aResolution
    );
    static const char *TAG;

    void SetTargetSpeed(
        uint16_t aSpeed, 
        uint16_t anAccelerationRate, 
        uint16_t aDecelerationRate) {
        myTargetSpeed = aSpeed;
        myAccelerationRate = anAccelerationRate;
        myDecelerationRate = aDecelerationRate;
        myFullSpeedAccelerationSteps = MovePlanner::CalculateSamples(0, myTargetSpeed, myAccelerationRate);
        myFullSpeedDecellerationSteps = MovePlanner::CalculateSamples(0, myTargetSpeed, myDecelerationRate);
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
    void Accelerate(uint32_t inSteps);

    /**
     * @brief execute the passed number of steps at the current speed
     * @param uint32_t numSteps // number of steps to execute
    */
    void MoveUniformSteps(uint32_t numSteps);

    /**
     * @brief accelerate to the target speed across the number of steps passed
     * @param uint32_t inSteps // The number of steps to spread the acceleration over
    */
    void Decelerate(uint32_t inSteps);

    gpio_num_t myStepPin;
    gpio_num_t myDirPin;
    gpio_num_t myEnPin;
    uint8_t myCWDir;
    uint8_t myCCWDir;
    uint8_t myEnableLevel;
    uint32_t myResolution;
    uint64_t myTargetPosition;
    uint64_t myCurrentPosition;
    uint16_t myTargetSpeed; //steps per second
    uint16_t myCurrentSpeed;
    uint16_t myAccelerationRate;
    uint16_t myDecelerationRate;
    uint8_t myDirection;
    bool myIsEnabled;
    uint32_t myFullSpeedAccelerationSteps; //Stores how many steps it takes to go from 0 to the current target speed
    uint32_t myFullSpeedDecellerationSteps; //Stores how many steps it takes to go from the current target speed to a full stop
    
    rmt_channel_handle_t myMotorChan;
    rmt_tx_channel_config_t myTxChanConfig;
    stepper_motor_curve_encoder_config_t myAccelEncoderConfig;
    rmt_encoder_handle_t myAccelEncoder;
    stepper_motor_uniform_encoder_config_t myUniformEncoderConfig;
    rmt_encoder_handle_t myUniformEncoder;
    stepper_motor_curve_encoder_config_t myDecelEncoderConfig;
    rmt_encoder_handle_t myDecelEncoder;
    rmt_transmit_config_t myTxConfig;
    //todo calculate these based off of speed and accel per time unit
    uint32_t myAccelSamples;
    uint32_t myUniformSpeedHz;
    uint32_t myDecelSamples;

};