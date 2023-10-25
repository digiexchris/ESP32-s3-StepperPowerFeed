#ifndef MACHINE_H
#define MACHINE_H

#include <mutex>
#include "Axis.h"
#include <memory>
#include <unordered_map>
class Machine
{
public:
    typedef std::unordered_map<char,Axis> Axes;

private:
    static Machine * instance;
    static std::mutex mutex;
    std::shared_ptr<FastAccelStepperEngine> engine;

protected:
    Machine();
    ~Machine() {}

    
    std::shared_ptr<Axes> axes;

public:

    /**
     * Singletons should not be cloneable.
     */
    Machine(Machine &other) = delete;

    Axis AddAxis(
        const char aLabel,
        gpio_num_t enPin, 
        gpio_num_t dirPin, 
        gpio_num_t stepPin, 
        StepperDriver::Level enableLevel, 
        uint32_t motorResolutionHz, 
        StepperDriver::StepperDirection startupMotorDirection
    );
    /**
     * Singletons should not be assignable.
     */
    void operator=(const Machine &) = delete;
    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    static Machine *GetInstance();
    std::shared_ptr<Machine::Axes> GetAxes() const;
};

class MachineException : std::exception {
    public:
        std::string myMsg;
        MachineException(const char* aMsg) {
            myMsg = aMsg;
        }
        virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override {
            return myMsg.c_str();
        }

};

#endif