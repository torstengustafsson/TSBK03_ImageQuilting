find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})


IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(OperatingSystem "Linux")
    set(EXT_FILES "Linux/MicroGlut.c")

    find_package(X11 REQUIRED)
    if (X11_FOUND)
        include_directories(${X11_INCLUDE_DIRS})
        link_libraries(${X11_LIBRARIES})
    endif()

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(OperatingSystem "Mac OS X")
    set(EXT_FILES "Mac/MicroGlut.m")

    SET(EXTRA_LIBS "-framework Cocoa -framework OpenGL -framework IOKit -framework Foundation -framework AppKit")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_library(COMMON STATIC GL_utilities.c loadobj.c LoadTGA.c VectorUtils3.c zpr.c ${EXT_FILES})

if(${OperatingSystem} MATCHES "Linux")
    target_link_libraries(COMMON m )
elseif(${OperatingSystem} MATCHES "Darwin")
    target_link_libraries(COMMON ${OPENGL_LIBRARIES} ${EXTRA_LIBS})
endif()
