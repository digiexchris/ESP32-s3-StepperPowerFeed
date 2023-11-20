#include <mutex>
#include <vector>
#include <memory>
#include "Axis.h"
#include "Machine.h"
#include <exception>
#include <fmt/core.h>

/**
 * Static methods should be defined outside the class.
 */

Machine* Machine::instance{nullptr};
std::mutex Machine::mutex;

Machine::Machine() {
    init = random();
    // engine = std::make_shared<FastAccelStepperEngine>();
    axes = std::make_shared<Axes>();
}

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */
Machine *Machine::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Machine();
    }
    return instance;
}

std::shared_ptr<Machine::Axes> Machine::GetAxes() const{
        return axes;
    } 

Axis Machine::AddAxis(
            const char aLabel,
        gpio_num_t enPin, 
        gpio_num_t dirPin, 
        gpio_num_t stepPin, 
        StepperDriver::Level enableLevel, 
        uint32_t stepperMaxStepsPerSecond,
        StepperDriver::StepperDirection startupMotorDirection,
        uint32_t rmtResolutionHz
) {
    if(axes->find(aLabel) != axes->end()) {
        throw std::invalid_argument(fmt::format("Axis {} already exists", aLabel));
    }

    Axis newAxis(
        aLabel,
        // engine,
        enPin, 
        dirPin, 
        stepPin, 
        enableLevel, 
        stepperMaxStepsPerSecond, 
        startupMotorDirection,
        rmtResolutionHz
    );

    //todo this mem for newAxis might be going out of scope after this returns, check that it copies or moves instead of nothing...
    //if it copies, returning newAxis not valid but from the returned iterator might be.
    auto result = axes->insert(Axes::value_type(aLabel, newAxis));

    if (result.second) {
        return result.first->second;
    }
    else
    {
        throw MachineException("Unable to insert Axis");
        abort();
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