#pragma once

#include <rmt_encoder.hpp>
/**
 * @brief Stepper motor curve encoder configuration
 */
// typedef struct {
//     uint32_t resolution;    // Encoder resolution, in Hz
//     uint32_t sample_points; // Sample points used for deceleration phase. Note: |end_freq_hz - start_freq_hz| >= sample_points
//     uint32_t start_freq_hz; // Start frequency on the curve, in Hz
//     uint32_t end_freq_hz;   // End frequency on the curve, in Hz
// } stepper_motor_curve_encoder_config_t;

/**
 * @brief Stepper motor uniform encoder configuration
 */
// typedef struct {
//     uint32_t resolution; // Encoder resolution, in Hz
// } stepper_motor_uniform_encoder_config_t;


class StepperMotorEncoder : public espp::RmtEncoder {
    public:

    //size_t EncodeFn(rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)


        static size_t EncodeFn(rmt_channel_handle_t channel, rmt_encoder_t *copy_encoder,
                               rmt_encoder_t *bytes_encoder, const void *primary_data,
                               size_t data_size, rmt_encode_state_t *ret_state);

        static esp_err_t DelFn(rmt_encoder_t * encoder) {
            return ESP_OK;
        };

        static esp_err_t ResetFn(rmt_encoder_t * encoder) {
            //todo: delete all queued pulses, MAYBE. maybe nothing.

            return ESP_OK;
        };
    
    
};

class CurvenEncoder : public StepperMotorEncoder {
    public:
        /**
         * @brief Create stepper motor curve encoder for acceleration phase
         *
         * @param[in] config Encoder configuration
         * @return
         *      - ESP_ERR_INVALID_ARG for any invalid arguments
         *      - ESP_ERR_NO_MEM out of memory when creating step motor encoder
         *      - ESP_OK if creating encoder successfully
         */
        // CurvenEncoder(const stepper_motor_curve_encoder_config_t *config);
};

class UniformEncoder : public StepperMotorEncoder {
    public:
        /**
         * @brief Create RMT encoder for encoding step motor uniform phase into RMT symbols
         *
         * @param[in] config Encoder configuration
         * @return
         *      - ESP_ERR_INVALID_ARG for any invalid arguments
         *      - ESP_ERR_NO_MEM out of memory when creating step motor encoder
         *      - ESP_OK if creating encoder successfully
         */
    //    static size_t EncodeFn(rmt_channel_handle_t channel, rmt_encoder_t *copy_encoder,
    //                            rmt_encoder_t *bytes_encoder, const void *primary_data,
    //                            size_t data_size, rmt_encode_state_t *ret_state);
};

