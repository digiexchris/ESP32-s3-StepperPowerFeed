#fastaccelstepper seems to have a problem > 5.0

set(min_supported_idf_version "4.4.4")
set(min_unsupported_idf_version "5.0.0")
set(idf_version "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")

if ("${idf_version}")
  if (idf_version VERSION_LESS min_supported_idf_version)
    message(FATAL_ERROR "This can be used with ESP-IDF versions "
                        "above or equal to ${min_supported_idf_version} and below ${min_unsupported_idf_version}, "
                        "but an older version is detected: ${idf_version}.")
  endif()
  if (idf_version VERSION_GREATER max_supported_idf_version)
    message(FATAL_ERROR "Arduino-esp32 can be used with ESP-IDF versions "
                        "above or equal to ${min_supported_idf_version} and below ${min_unsupported_idf_version}, "
                        "but a newer version is detected: ${idf_version}.")
  endif()
endif()

idf_component_register(SRCS "Stepper.cpp" "main.cpp" "Axis.cpp" "Machine.cpp"
                    INCLUDE_DIRS "." REQUIRES "RMTAccelerationStepper" "hal" "driver" "fmt")