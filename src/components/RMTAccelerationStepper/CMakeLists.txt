

set(EXTRA_COMPONENT_DIRS
"components/espp/components"
)

idf_component_register(SRCS "RMTStepper.cpp" "stepper_motor_encoder.c"
    PRIV_REQUIRES "hal" "driver" "MovePlanner" "Exception" "state_machine"
                       INCLUDE_DIRS ".")
