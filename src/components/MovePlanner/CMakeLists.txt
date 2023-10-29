
if(SDKCONFIG)
    #started via ESP_IDF toolchain
    idf_component_register(SRCS "MovePlanner.cpp"
    PRIV_REQUIRES "Exception" INCLUDE_DIRS ".")
else()
    #probably used in win64 stuff

    add_library(MovePlanner
        MovePlanner.cpp
    )
    
    #target_include_directories(${PROJECT_NAME} PUBLIC . "${component_dir}/Exception/")
    target_include_directories(MovePlanner PUBLIC . "../Exception/")
endif()