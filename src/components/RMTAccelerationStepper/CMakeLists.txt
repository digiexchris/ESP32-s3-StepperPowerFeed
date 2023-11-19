

# set(EXTRA_COMPONENT_DIRS
# "../../lib/espp/components"
# )

idf_component_register(SRCS "RMTStepper.cpp" "StepperMotorEncoder.cpp"
    PRIV_REQUIRES "hal" "driver" "MovePlanner" "Exception" "StateMachine" "rmt" "task"
                       INCLUDE_DIRS ".")
