if(SDKCONFIG)
    idf_component_register( SRC "Exception.cpp" INCLUDE_DIRS ".")
else()
    add_library(Exception Exception.cpp)
    set(Exception_PUBLIC_HEADERS Exception.h)

    message("Current Source Directory for Exception: ${CMAKE_CURRENT_SOURCE_DIR}")

    target_include_directories(Exception PUBLIC .)
    get_property(dirs TARGET Exception PROPERTY INCLUDE_DIRECTORIES)

    message("Include directories for Exception are: ${dirs}")
endif()