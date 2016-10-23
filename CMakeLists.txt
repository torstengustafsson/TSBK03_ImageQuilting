cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME ImageQuilting)

project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

find_package(OpenGL REQUIRED)

include_directories("../common" )
include_directories(${OPENGL_INCLUDE_DIR})

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include_directories("../common/Linux")
    add_definitions(-DGL_GLEXT_PROTOTYPES)
    SET(OperatingSystem "Linux")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")


IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include_directories("../common/Mac")
    SET(OperatingSystem "Mac OS X")

    SET(EXTRA_LIBS "-framework Cocoa -framework OpenGL -framework IOKit -framework Foundation -framework AppKit")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_subdirectory(../common COMMON)

set(SOURCE_FILES
       include/main.cpp include/ImageQuilter.cpp include/Square.cpp include/RandomGenerator.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${EXTRA_LIBS} COMMON )
