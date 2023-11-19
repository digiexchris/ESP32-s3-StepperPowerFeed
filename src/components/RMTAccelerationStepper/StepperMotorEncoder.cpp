#include "esp_check.h"
#include "StepperMotorEncoder.h"

static const char *TAG = "stepper_motor_encoder";

static float convert_to_smooth_freq(uint32_t freq1, uint32_t freq2, uint32_t freqx)
{
    float normalize_x = ((float)(freqx - freq1)) / (freq2 - freq1);
    // third-order "smoothstep" function: https://en.wikipedia.org/wiki/Smoothstep
    float smooth_x = normalize_x * normalize_x * (3 - 2 * normalize_x);
    return smooth_x * (freq2 - freq1) + freq1;
}

// size_t CurveEncoder::rmt_encode_stepper_motor_curve(rmt_channel_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
// {
//     rmt_stepper_curve_encoder_t *motor_encoder = __containerof(&_encoder, rmt_stepper_curve_encoder_t, base);
//     rmt_encoder_handle_t copy_encoder = motor_encoder->copy_encoder;
//     rmt_encode_state_t session_state = RMT_ENCODING_RESET;
//     uint32_t points_num = *(uint32_t *)primary_data;
//     size_t encoded_symbols = 0;
//     if (motor_encoder->flags.is_accel_curve) {
//         encoded_symbols = copy_encoder->encode(copy_encoder, channel, &motor_encoder->curve_table[0],
//                                                points_num * sizeof(rmt_symbol_word_t), &session_state);
//     } else {
//         encoded_symbols = copy_encoder->encode(copy_encoder, channel, &motor_encoder->curve_table[0] + motor_encoder->sample_points - points_num,
//                                                points_num * sizeof(rmt_symbol_word_t), &session_state);
//     }
//     *ret_state = session_state;
//     return encoded_symbols;
// }

// esp_err_t CurveEncoder::rmt_del_stepper_motor_curve_encoder()
// {
//     rmt_stepper_curve_encoder_t *motor_encoder = __containerof(&_encoder, rmt_stepper_curve_encoder_t, base);
//     rmt_del_encoder(motor_encoder->copy_encoder);
//     free(motor_encoder);
//     return ESP_OK;
// }

// esp_err_t CurveEncoder::rmt_reset_stepper_motor_curve_encoder()
// {
//     rmt_stepper_curve_encoder_t *motor_encoder = __containerof(&_encoder, rmt_stepper_curve_encoder_t, base);
//     rmt_encoder_reset(motor_encoder->copy_encoder);
//     return ESP_OK;
// }

// CurvenEncoder::CurvenEncoder(const stepper_motor_curve_encoder_config_t *config)
// {
//     esp_err_t ret = ESP_OK;
//     rmt_stepper_curve_encoder_t *step_encoder = NULL;
//     float smooth_freq;
//     uint32_t symbol_duration;
//     ESP_GOTO_ON_FALSE(config, ESP_ERR_INVALID_ARG, err, TAG, "invalid arguments");
//     ESP_GOTO_ON_FALSE(config->sample_points, ESP_ERR_INVALID_ARG, err, TAG, "sample points number can't be zero");
//     ESP_GOTO_ON_FALSE(config->start_freq_hz != config->end_freq_hz, ESP_ERR_INVALID_ARG, err, TAG, "start freq can't equal to end freq");
//     step_encoder = (rmt_stepper_curve_encoder_t*)calloc(1, sizeof(rmt_stepper_curve_encoder_t) + config->sample_points * sizeof(rmt_symbol_word_t));
//     ESP_GOTO_ON_FALSE(step_encoder, ESP_ERR_NO_MEM, err, TAG, "no mem for stepper curve encoder");
//     rmt_copy_encoder_config_t copy_encoder_config = {};
//     ESP_GOTO_ON_ERROR(rmt_new_copy_encoder(&copy_encoder_config, &step_encoder->copy_encoder), err, TAG, "create copy encoder failed");
//     bool is_accel_curve = config->start_freq_hz < config->end_freq_hz;

//     // prepare the curve table, in RMT symbol format
//     uint32_t curve_step = 0;
//     if (is_accel_curve) {
//         curve_step = (config->end_freq_hz - config->start_freq_hz) / (config->sample_points - 1);
//         for (uint32_t i = 0; i < config->sample_points; i++) {
//             smooth_freq = convert_to_smooth_freq(config->start_freq_hz, config->end_freq_hz, config->start_freq_hz + curve_step * i);
//             symbol_duration = config->resolution / smooth_freq / 2;
//             step_encoder->curve_table[i].level0 = 0;
//             step_encoder->curve_table[i].duration0 = symbol_duration;
//             step_encoder->curve_table[i].level1 = 1;
//             step_encoder->curve_table[i].duration1 = symbol_duration;
//         }
//     } else {
//         curve_step = (config->start_freq_hz - config->end_freq_hz) / (config->sample_points - 1);
//         for (uint32_t i = 0; i < config->sample_points; i++) {
//             smooth_freq = convert_to_smooth_freq(config->end_freq_hz, config->start_freq_hz, config->end_freq_hz + curve_step * i);
//             symbol_duration = config->resolution / smooth_freq / 2;
//             step_encoder->curve_table[config->sample_points - i - 1].level0 = 0;
//             step_encoder->curve_table[config->sample_points - i - 1].duration0 = symbol_duration;
//             step_encoder->curve_table[config->sample_points - i - 1].level1 = 1;
//             step_encoder->curve_table[config->sample_points - i - 1].duration1 = symbol_duration;
//         }
//     }

//     step_encoder->base.encode = StepperMotorEncoder::rmt_encode_stepper_motor_curve;
//     step_encoder->base.del = StepperMotorEncoder::rmt_del_stepper_motor_curve_encoder;
//     step_encoder->base.reset = StepperMotorEncoder::rmt_reset_stepper_motor_curve_encoder;
//     step_encoder->sample_points = config->sample_points;
//     step_encoder->flags.is_accel_curve = is_accel_curve;
//     _encoder = step_encoder;
//     return ESP_OK;

// // err:
// //     if (step_encoder) {
// //         rmt_del_encoder(step_encoder->copy_encoder);
// //         free(step_encoder);
// //     }
// //     return ret;
// // }
//     //     }
//     // }
//     ESP_GOTO_ON_FALSE(curve_step > 0, ESP_ERR_INVALID_ARG, err, TAG, "|end_freq_hz - start_freq_hz| can't be smaller than sample_points");

//     step_encoder->sample_points = config->sample_points;
//     step_encoder->flags.is_accel_curve = is_accel_curve;
//     step_encoder->base.del = rmt_del_stepper_motor_curve_encoder;
//     step_encoder->base.encode = rmt_encode_stepper_motor_curve;
//     step_encoder->base.reset = rmt_reset_stepper_motor_curve_encoder;
//     *ret_encoder = &(step_encoder->base);
//     return ESP_OK;
// // err:
// //     if (step_encoder) {
// //         if (step_encoder->copy_encoder) {
// //             rmt_del_encoder(step_encoder->copy_encoder);
// //         }
// //         free(step_encoder);
// //     }
//     return ret;
// }

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_handle_t copy_encoder;
    uint32_t resolution;
} rmt_stepper_uniform_encoder_t;

size_t StepperMotorEncoder::EncodeFn(
    rmt_channel_handle_t channel, 
    rmt_encoder_t *copy_encoder,
    rmt_encoder_t *bytes_encoder, 
    const void *primary_data,
    size_t data_size, 
    rmt_encode_state_t *ret_state
    )
{
    // rmt_stepper_uniform_encoder_t *motor_encoder = __containerof(encoder, rmt_stepper_uniform_encoder_t, base);
    // rmt_encoder_handle_t copy_encoder = motor_encoder->copy_encoder;
    // rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    // uint32_t target_freq_hz = *(uint32_t *)primary_data;
    // uint32_t symbol_duration = motor_encoder->resolution / target_freq_hz / 2;
    // rmt_symbol_word_t freq_sample = {
    //     .level0 = 0,
    //     .duration0 = symbol_duration,
    //     .level1 = 1,
    //     .duration1 = symbol_duration,
    // };
    // size_t encoded_symbols = copy_encoder->encode(copy_encoder, channel, &freq_sample, sizeof(freq_sample), &session_state);
    // *ret_state = session_state;
    return 1;//encoded_symbols;
}

// UniformEncoder::UniformEncoder(const Config *config)
// {

//     myBytesEncoderConfig = {
//         .rmt_mode = RMT_MODE_TX,
//         .channel = myMotorChan,
//         .gpio_num = myStepPin,
//         .clk_div = 80, // 1us per tick
//         .mem_block_num = 1,
//         .tx_config = {
//             .loop_en = false,
//             .carrier_freq_hz = 0,
//             .carrier_level = RMT_CARRIER_LEVEL_LOW,
//             .carrier_duty_percent = 0,
//             .carrier_en = false,
//             .idle_level = RMT_IDLE_LEVEL_LOW,
//             .idle_output_en = true,
//         },
//     };
//     }
//     esp_err_t ret = ESP_OK;
//     rmt_stepper_uniform_encoder_t *step_encoder = NULL;
//     ESP_GOTO_ON_FALSE(config && ret_encoder, ESP_ERR_INVALID_ARG, err, TAG, "invalid arguments");
//     step_encoder = calloc(1, sizeof(rmt_stepper_uniform_encoder_t));
//     ESP_GOTO_ON_FALSE(step_encoder, ESP_ERR_NO_MEM, err, TAG, "no mem for stepper uniform encoder");
//     rmt_copy_encoder_config_t copy_encoder_config = {};

//     resolution = config->resolution;


//     ESP_GOTO_ON_ERROR(rmt_new_copy_encoder(&copy_encoder_config, &step_encoder->copy_encoder), err, TAG, "create copy encoder failed");

//     step_encoder->resolution = config->resolution;
//     step_encoder->base.del = rmt_del_stepper_motor_uniform_encoder;
//     step_encoder->base.encode = rmt_encode_stepper_motor_uniform;
//     step_encoder->base.reset = rmt_reset_stepper_motor_uniform;
//     *ret_encoder = &(step_encoder->base);
//     return ESP_OK;
// err:
//     if (step_encoder) {
//         if (step_encoder->copy_encoder) {
//             rmt_del_encoder(step_encoder->copy_encoder);
//         }
//         free(step_encoder);
//     }
//     return ret;
// }
