cmake_minimum_required(VERSION 3.8)
project(PS1_files)

set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

set(SOURCE_FILES
        bitmap.c
        bitmap.h
        julia_mpi.c
        julia_mpi.h)

add_executable(PS1_files ${SOURCE_FILES})
target_link_libraries(PS1_files ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
    set_target_properties(PS1_files PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(PS1_files PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()