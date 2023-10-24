#include <mutex>
#include <vector>
#include <memory>
#include "Axis.h"
#include "Machine.h"
#include <exception>
#include <format>

Machine::Machine() {
    engine = std::make_shared<FastAccelStepperEngine>();
    axes = std::make_shared<Axes>();
}

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */
Machine *Machine::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance == nullptr)
    {
        instance = new Machine();
    }
    return instance;
}

Axes* Machine::GetAxes() const{
        return axes;
    } 

Axis Machine::AddAxis(
    const char aLabel,
    gpio_num_t enPin, 
    gpio_num_t dirPin, 
    gpio_num_t stepPin, 
    StepperDriver::Level enableLevel, 
    uint32_t motorResolutionHz, 
    StepperDriver::StepperDirection startupMotorDirection
) {
    if(axes.find(aLabel) != axes.end()) {
        throw std::invalid_argument(std::format("Axis {} already exists", aLabel));
    }

    axes.insert(Axes::value_type(aLabel,
        new Axis(
            aLabel,
            engine,
            enPin, 
            dirPin, 
            stepPin, 
            enableLevel, 
            motorResolutionHz, 
            startupMotorDirection
        ))
    );

    //Find a better way, there are 2 lookups and an insert in here. Sloppy.
    if (Axis axis = axes.find(aLabel); axis != axes.end()) {
        return axis;
    }
    else
    {
        //todo throw a not found exception here
        throw std::exception("Unable to insert Axis")
    }
}

// void ThreadFoo(){
//     // Following code emulates slow initialization.
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     Singleton* singleton = Singleton::GetInstance("FOO");
//     std::cout << singleton->value() << "\n";
// }

// void ThreadBar(){
//     // Following code emulates slow initialization.
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     Singleton* singleton = Singleton::GetInstance("BAR");
//     std::cout << singleton->value() << "\n";
// }