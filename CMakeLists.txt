cmake_minimum_required(VERSION 3.11)
project ("ucos3-posix")
  
set(TARGET_EXE "ucos3-posix")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -static -O0 -g3 -Wall -fmessage-length=0  ")  #set debug mode c flags
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall ")         #set release mode c flags
set(CMAKE_BUILD_TYPE "Debug")     #set Debug or Release

### INCLUDE DIR ###
# Application
include_directories(${CMAKE_SOURCE_DIR}/app/Posix/GNU/OS3)

# Ucos-iii Kernel
include_directories(${CMAKE_SOURCE_DIR}/kernel/uC-CPU)
include_directories(${CMAKE_SOURCE_DIR}/kernel/uC-CPU/Posix/GNU)  
include_directories(${CMAKE_SOURCE_DIR}/kernel/uC-LIB)
include_directories(${CMAKE_SOURCE_DIR}/kernel/uCOS-III/Ports/Posix/GNU)
include_directories(${CMAKE_SOURCE_DIR}/kernel/uCOS-III/Source)


### SUB CMAKE DIR ###
# Ucos-iii Kernel
add_subdirectory(kernel/uC-CPU/Posix/GNU)
add_subdirectory(kernel/uC-CPU)
add_subdirectory(kernel/uCOS-III/Source)
add_subdirectory(kernel/uCOS-III/Ports/Posix/GNU)  
add_subdirectory(kernel/uC-LIB)
  
AUX_SOURCE_DIRECTORY(app/Posix/GNU/OS3 APP_SRC)

set(EXERCISES_BASE_DIR ${CMAKE_SOURCE_DIR}/app/Posix/GNU/OS3)

set(SHARED_SOURCES
    ${EXERCISES_BASE_DIR}/utils.c
)

file(GLOB EXERCISE_DIRS ${EXERCISES_BASE_DIR}/Exercise-*)

foreach(EXERCISE_DIR ${EXERCISE_DIRS})
    if(IS_DIRECTORY ${EXERCISE_DIR})
        get_filename_component(EXAMPLE_NAME ${EXERCISE_DIR} NAME)

        add_executable(${EXAMPLE_NAME} ${EXERCISE_DIR}/app.c ${SHARED_SOURCES})

        target_include_directories(${EXAMPLE_NAME} PRIVATE ${EXAMPLES_BASE_DIR})

        target_link_libraries(${EXAMPLE_NAME} ucos-port ucos uclib cpu cpu-arch pthread)

        message(STATUS "Added executable: ${EXAMPLE_NAME}")
    endif()
endforeach()