#pragma once
#include <StateMachine.hpp>

// Derived State classes
class AcceleratingState : public SM::State {
    // Specific attributes and methods for AcceleratingState
};

class RunningState : public SM::State {
    // Specific attributes and methods for RunningState
};

class DeceleratingState : public SM::State {
    // Specific attributes and methods for DeceleratingState
};

class StoppedState : public SM::State {
    // Specific attributes and methods for StoppedState
};