cmake_minimum_required(VERSION 3.23)
project(dcsim)

set(CMAKE_CXX_STANDARD 23)

include_directories(extern include)

add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++)

include(cmake/CPM.cmake)

add_library(dcsim
    src/level.cpp
    src/wire.cpp
    src/chip.cpp
    src/circuit.cpp
    )
target_include_directories(dcsim PUBLIC include)
target_compile_features(dcsim PUBLIC cxx_std_23)

add_executable(main main.cpp)
target_link_libraries(main dcsim)
add_subdirectory(tests)