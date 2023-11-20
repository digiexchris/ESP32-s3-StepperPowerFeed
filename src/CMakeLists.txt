# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
# set(CMAKE_CXX_CPPCHECK "cppcheck")
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS
"lib/espp/components"
)

set(
  COMPONENTS
  "main rmt task"
  CACHE STRING
  "List of components to include"
  )

project(StepperPowerFeed)

add_compile_options(-Wno-reorder)