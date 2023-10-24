#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Stepper.h"
#include <memory>
#include "Machine.h"
#include "Axis.h"

#define SUPPORT_SELECT_DRIVER_TYPE
//#define SUPPORT_ESP32_RMT

using namespace StepperDriver;

extern "C" void app_main(void)
{

    Machine* machine = Machine::GetInstance();

    Axis axis = machine->AddAxis(
        'x',
        GPIO_NUM_0,
        GPIO_NUM_1,
        GPIO_NUM_2,
        Level::GPIO_LOW, 
        1000000, 
        StepperDirection(Direction::CW, Level::GPIO_LOW)
    );

    //disabled for servo motor
    axis.stepper->SetAcceleration(0);
    axis.stepper->SetSpeed(1, Stepper::DistancePerTimeUnit::IPM);
}

void vTaskFunction( void * pvParameters ) { 
    //Block for 500ms. 
    const TickType_t xDelay = 2500 / portTICK_PERIOD_MS; 
    for( ;; ) 
    { 
        Machine::Axes* axes = Machine::GetInstance()->GetAxes();
        //todo this find is clunky, make a Machine::GetInstance()->GetAxis(const char*) func instead
        Axis xAxis = axes->find('x')->second;

        xAxis.stepper->SetSpeed(1, Stepper::DistancePerTimeUnit::IPM);
        xAxis.stepper->Run(Direction::CW);
        vTaskDelay( xDelay );
        xAxis.stepper->Run(Direction::CCW);
        vTaskDelay( xDelay );
        xAxis.stepper->Stop();
        vTaskDelay( xDelay );
    } 
}