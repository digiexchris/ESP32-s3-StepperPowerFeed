#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <queue>
#include <memory>
#include <mutex>
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <typeindex>

namespace SM {

// // Define the states
// enum class State {
//     ACCELERATING,
//     RUNNING,
//     DECELERATING,
//     STOPPED
// };

// enum class EventType {
//         ACCELERATE,
//         RUN,
//         DECELERATE,
//         STOP,
//         CHANGE_DIRECTION,
//         CHANGE_SPEED
//     };

class State {
public:
    virtual ~State() = default;
};

// Base Event class
class Event {
public:
    virtual ~Event() = default;
};

// Define a type for the function that handles state transitions
using TransitionHandler = std::function<void(const State&, const Event&)>;

// Define a type for the map that associates event types with their handlers
using EventMap = std::unordered_map<std::type_index, TransitionHandler>;

// Finally, define the TransitionMap type
using TransitionMap = std::unordered_map<std::type_index, EventMap>;

// StateMachine class
class StateMachine {
protected:
    std::unique_ptr<TransitionMap> myTransitionMap;
    std::unique_ptr<State> myCurrentState;
    std::queue<std::shared_ptr<Event>> eventQueue;
    std::mutex queueMutex;

public:

    //remember to set your transition map
    StateMachine() = default;

    StateMachine(std::unique_ptr<TransitionMap> aTransitionmap, std::unique_ptr<State> anInitialState) : myCurrentState(std::move(anInitialState))
    {
        // Initialize the transition map
        // For example: transitionMap[typeid(AcceleratingState)][typeid(AccelerateEvent)] = 
        myTransitionMap = std::move(aTransitionmap);
        myCurrentState = std::move(anInitialState);
    }

    void AddEvent(std::shared_ptr<Event> event) {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventQueue.push(event);
    }

    virtual void HandleEvent(const Event& event) = 0;

    void ProcessEvents() {
        while (true) {
            std::shared_ptr<Event> event;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (eventQueue.empty()) {
                    break;
                }
                event = eventQueue.front();
                eventQueue.pop();
            }
            HandleEvent(*event);
        }
    }

    

    State GetCurrentState() const {
        return *myCurrentState;
    }
};

} // namespace StateMachine