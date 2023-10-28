#pragma once
#include <hal/gpio_types.h>
#include "stepper_motor_encoder.h"
#include "driver/rmt_tx.h"
#include "RMTStepper.h"

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
    }

    void SetDirection(uint8_t aDirection) {
        //use the FSM to coordinate this properly. Shouldn't reverse if not stopped first.
        myDirection = aDirection;
        ESP_LOGI(TAG, "Set spin direction");
        gpio_set_level(myDirPin, myDirection);
    }

    void Move(uint64_t aStepsToMove) {
        //if this is < current position, use the FSM to reverse it.
        //calculate number of required samples for acceleration
        //currently is just from 0 speed to target and back to zero.
        //make it go to a new target speed instead of 0
        Accelerate(#);
        MoveUniformSteps(#);
        Decelerate(#);
    }

    /**
     * @brief calculate the number of samples required to go from the current speed
     * to the target speed, if possible within the number of samples available
     * keeping it bound within the available acceleration/deceleration rate available.
     * eg. if we can accelerate 10s^2, and we only have 2 samples available, and we're starting at
     * 0 speed, and the target speed is 100, we'll only reach 20/sec speed. Number of samples is therefore 
     * capped at 2. NOTE: this is probably an error condition, since there are no moves available to
     * decelerate. detect that in the calling function.
     * 
     * If we can do the same acceleration, but we have 1500 steps available,
     * we will require 10 steps to get up to speed, so return 10. 
     * //todo, check that math,
     * it's late right now.
    */
    void CalculateSamples(uint32_t aSpeedDelta, uint32_t aRate, uint32_t aTotalAvailableSteps);

    void Run() {
        //keep adding to the queue as the queue gets consumed I guess?
    }

    void Stop() {
        //empty the uniform queue and decelerate using the FSM
    }

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