cmake_minimum_required(VERSION 3.15)
project(MantraLang VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/W4 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(mantra_vm STATIC
    src/vm/value.cpp
    src/vm/stack.cpp
    src/vm/bytecode.cpp
    src/vm/vm.cpp
)

target_include_directories(mantra_vm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(mantra main.cpp)
target_link_libraries(mantra PRIVATE mantra_vm)

enable_testing()

add_executable(mantra_vm_tests tests/test_vm.cpp)
target_link_libraries(mantra_vm_tests PRIVATE mantra_vm)
add_test(NAME mantra_vm_tests COMMAND mantra_vm_tests)

message(STATUS "MANTRA VM Foundation")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
