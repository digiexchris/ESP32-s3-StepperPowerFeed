

# set(EXTRA_COMPONENT_DIRS
# "../../lib/espp/components"
# )

idf_component_register(SRCS "RMTStepper.cpp" "StepperMotorEncoder.cpp"
    PRIV_REQUIRES "hal" "driver" "MovePlanner" "Exception" "state_machine" "rmt"
                       INCLUDE_DIRS ".")
