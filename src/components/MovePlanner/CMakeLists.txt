
if(SDKCONFIG)
    #started via ESP_IDF toolchain
    idf_component_register(SRCS "MovePlanner.cpp"
    PRIV_REQUIRES "Exception" INCLUDE_DIRS ".")
else()
    #probably used in win64 stuff

    add_library(MovePlanner
        MovePlanner.cpp
    )

    message("Current Source Directory for MovePlanner: ${CMAKE_CURRENT_SOURCE_DIR}")
    get_filename_component(Exception_ABSOLUTE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Exception" ABSOLUTE)
    target_include_directories(MovePlanner PUBLIC . ${Exception_ABSOLUTE_DIR})
    get_property(dirs TARGET MovePlanner PROPERTY INCLUDE_DIRECTORIES)
    message("Include directories for MovePlanner are: ${dirs}")

    target_link_libraries(MovePlanner Exception)
endif()