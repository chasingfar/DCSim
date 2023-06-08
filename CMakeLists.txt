cmake_minimum_required(VERSION 3.23)
project(dcsim)

set(CMAKE_CXX_STANDARD 23)

include_directories(extern include)

add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++)

include(cmake/CPM.cmake)

add_library(libdcsim
    src/level.cpp
    src/wire.cpp
    src/chip.cpp
    src/circuit.cpp
    )
target_include_directories(libdcsim PUBLIC include)
target_compile_features(libdcsim PUBLIC cxx_std_23)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_executable(dcsim main.cpp)
    target_link_libraries(dcsim libdcsim)
    
    add_subdirectory(tests)
endif()