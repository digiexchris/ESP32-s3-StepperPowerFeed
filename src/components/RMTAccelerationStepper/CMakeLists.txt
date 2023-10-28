idf_component_register(SRCS "rmt_stepper_motor.cpp" "stepper_motor_encoder.c"
    PRIV_REQUIRES "hal" "driver"
                       INCLUDE_DIRS ".")
