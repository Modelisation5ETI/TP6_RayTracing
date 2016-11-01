cmake_minimum_required(VERSION 2.6)

project( Raytracing )

# Configure Project
set(CMAKE_BUILD_TYPE Debug)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

set( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )

include_directories( ${SRC_DIR} )

# add_executable sources
file( GLOB_RECURSE
  source_files
  ${SRC_DIR}/*.[cht]pp
  )

add_executable( ${PROJECT_NAME} ${source_files} )

add_definitions( -O2 -Wall -Wextra -std=c++11 -Wno-comment -Wno-unused-parameter -Wno-unused-function -DCORRECTION )

target_link_libraries( ${PROJECT_NAME} -lm -ldl -fopenmp )


