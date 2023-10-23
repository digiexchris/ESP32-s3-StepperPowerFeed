#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Stepper.h"

#define SUPPORT_SELECT_DRIVER_TYPE
//#define SUPPORT_ESP32_RMT
extern "C" void app_main(void)
{
    const static uint32_t accel_samples = 500; //number of pulses to send?
    const static uint32_t uniform_speed_hz = 1500; //not sure, why hz? steps per second maybe
    const static uint32_t decel_samples = 500;

    StepperDriver::Stepper stepper(
        GPIO_NUM_0,
        GPIO_NUM_1,
        GPIO_NUM_2,
        StepperDriver::Level::GPIO_LOW, 
        1000000, 
        StepperDriver::StepperDirection(StepperDriver::Direction::CW, StepperDriver::Level::GPIO_LOW)
    );

    //todo: Translate samples and uniform_speed_hz into pulses (I think), or distance.

    // while (1) {
    //     // acceleration phase
    //     tx_config.loop_count = 0;
    //     ESP_ERROR_CHECK(rmt_transmit(motor_chan, accel_motor_encoder, &accel_samples, sizeof(accel_samples), &tx_config));

    //     // uniform phase
    //     tx_config.loop_count = 5000; //this means repeat this 5000 steps at 1500 steps per second?
    //     ESP_ERROR_CHECK(rmt_transmit(motor_chan, uniform_motor_encoder, &uniform_speed_hz, sizeof(uniform_speed_hz), &tx_config));

    //     // deceleration phase
    //     tx_config.loop_count = 0;
    //     ESP_ERROR_CHECK(rmt_transmit(motor_chan, decel_motor_encoder, &decel_samples, sizeof(decel_samples), &tx_config));
    //     // wait all transactions finished
    //     ESP_ERROR_CHECK(rmt_tx_wait_all_done(motor_chan, -1));

    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}