idf_component_register(SRCS "RMTStepper.cpp" "stepper_motor_encoder.c"
    PRIV_REQUIRES "hal" "driver" "MovePlanner" "Exception"
                       INCLUDE_DIRS ".")
