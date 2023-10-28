#pragma once
#include <hal/gpio_types.h>
#include "stepper_motor_encoder.h"
#include "driver/rmt_tx.h"

class RMTStepper {
    public:
        RMTStepper(
            gpio_num_t aStepPin, 
            gpio_num_t aDirPin, 
            gpio_num_t anEnPin, 
            uint8_t aCWDirLevel, 
            uint8_t anEnableLevel,
            uint32_t aResolution
        );
        static const char *TAG;

    private:
        gpio_num_t myStepPin;
        gpio_num_t myDirPin;
        gpio_num_t myEnPin;
        uint8_t myCWDirLevel;
        uint8_t myCCWDirLevel;
        uint8_t myEnableLevel;
        uint32_t myResolution;
        
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