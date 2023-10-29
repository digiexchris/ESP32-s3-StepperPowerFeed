# CMakeList.txt : CMake project for win64_tests, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("win64_tests")
set(CMAKE_COLOR_DIAGNOSTICS ON)
get_filename_component(component_dir "../src/components" ABSOLUTE)
# Add source to this project's executable.
add_executable (win64_tests "win64_tests.cpp" "win64_tests.h"
	"../src/components/Exception/Exception.h" 
	"../src/components/Exception/Exception.cpp"
	"../src/components/MovePlanner/MovePlanner.h" 
	"../src/components/MovePlanner/MovePlanner.cpp"
	"./lib/Unity/src/unity.c"
)

# add_library(Unity STATIC)

add_subdirectory("${component_dir}/MovePlanner/" "./out/build/MovePlanner")
add_subdirectory("${component_dir}/Exception/" "./out/build/Exception")
add_subdirectory("lib/Unity/" "./out/build/Unity")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET win64_tests PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
