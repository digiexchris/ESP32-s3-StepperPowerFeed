#include <hal/gpio_types.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Stepper.h"
#include "Machine.h"
#include "Axis.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

using namespace StepperDriver;

void vTaskFunction( void * pvParameters ) { 
    const char* TAG = "vTaskFunc";
    ESP_LOGI(TAG,"Starting vTask Proper");
    //Block for 500ms. 
    const TickType_t xDelay = 2500 / portTICK_PERIOD_MS; 
    auto axes = Machine::GetInstance()->GetAxes();
    Axis xAxis = axes->find('x')->second;

    for( ;; ) 
    {
        
        //todo this find is clunky, make a Machine::GetInstance()->GetAxis(const char*) func instead
        

        xAxis.stepper->SetSpeed(1, Stepper::DistancePerTimeUnit::IPM);
        xAxis.stepper->Run(Direction::CW);
        vTaskDelay( xDelay );
        xAxis.stepper->Run(Direction::CCW);
        vTaskDelay( xDelay );
        xAxis.stepper->Stop();
        vTaskDelay( xDelay );
    } 
}

extern "C" void app_main(void)
{
    const char* TAG = "main";
    ESP_LOGI(TAG,"Starting");
    Machine* machine = Machine::GetInstance();
    ESP_LOGI(TAG,"Init Machine");

    Axis axis = machine->AddAxis(
        'x',
        GPIO_NUM_12,
        GPIO_NUM_13,
        GPIO_NUM_14,
        Level::GPIO_LOW, 
        1000000, 
        StepperDirection(Direction::CW, Level::GPIO_LOW)
    );
    ESP_LOGI(TAG,"Created Axis");
    //disabled for servo motor
    //axis.stepper->SetAcceleration(100);
    axis.stepper->SetSpeed(1, Stepper::DistancePerTimeUnit::IPM);
    // axis.stepper->Run(Direction::CCW);
    const TickType_t xDelay = 2500 / portTICK_PERIOD_MS;
    // vTaskDelay( xDelay );
    for(;;) {
        ESP_LOGI(TAG,"Moving 1000");
        axis.stepper->MoveDistance(1000);
        vTaskDelay( xDelay *4);
    }
    
    ESP_LOGI(TAG,"Starting Task");
    xTaskCreate(&vTaskFunction, "hello_task", 8000, NULL, 5, NULL);
    //xTaskCreatePinnedToCore(&vTaskFunction,"example",4096,NULL,1, NULL, tskNO_AFFINITY);
}