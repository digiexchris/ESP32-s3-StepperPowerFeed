#pragma once
#include <StateMachine.hpp>
#include <map>
#include <set>

// Define valid transitions for each state
// const std::map<State, std::set<EventType>> ValidTransitions = {
//     {State::ACCELERATING, {EventType::ACCELERATE, EventType::RUN, EventType::DECELERATE, EventType::STOP}},
//     {State::RUNNING, {EventType::ACCELERATE, EventType::RUN, EventType::DECELERATE, EventType::STOP}},
//     {State::DECELERATING, {EventType::ACCELERATE, EventType::RUN, EventType::DECELERATE, EventType::STOP}},
//     {State::STOPPED, {EventType::ACCELERATE, EventType::RUN, EventType::DECELERATE, EventType::STOP, EventType::CHANGE_DIRECTION, EventType::CHANGE_SPEED}}
// };

// Specific Event classes
class AccelerateEvent : public SM::Event { 
 };

class RunEvent : public SM::Event {
};

class DecelerateEvent : public SM::Event {
};

class StopEvent : public SM::Event {
public:
};

class ChangeDirectionEvent : public SM::Event {
public:
};

class ChangeSpeedEvent : public SM::Event {
public:
};