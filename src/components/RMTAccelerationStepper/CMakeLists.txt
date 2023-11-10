

set(EXTRA_COMPONENT_DIRS
"components/espp/components"
)

idf_component_register(SRCS "RMTStepper.cpp" "StepperMotorEncoder.c"
    PRIV_REQUIRES "hal" "driver" "MovePlanner" "Exception" "state_machine" "rmt"
                       INCLUDE_DIRS ".")
