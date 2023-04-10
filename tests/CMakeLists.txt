
cmake_minimum_required(VERSION 3.23)


CPMAddPackage("gh:onqtam/doctest@2.4.11")


add_executable(tests
    main.cpp
    level.cpp
    wire.cpp
    port.cpp
    circuit.cpp
    )
target_compile_features(tests PRIVATE cxx_std_23)
target_link_libraries(tests
    PUBLIC
        dcsim
    PRIVATE 
        doctest)

#enable_testing()
#add_test(tests tests)