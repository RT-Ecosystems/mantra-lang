cmake_minimum_required(VERSION 3.15)
project(MantraLang VERSION 0.1.0 
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(mantra_lib STATIC
  core/lexer.cpp
  core/parser.cpp
  core/interpreter.cpp
  core/repl.cpp
  error/error.cpp
  stdlib/io.cpp
  stdlib/math.cpp
  stdlib/string.cpp
  unicode/unicode.cpp
  unicode/script_detect.cpp
  compiler/compiler.cpp
  semantic/semantic_analyzer.cpp
  bytecode/chunk.cpp
  src/vm/value.cpp
  src/vm/stack.cpp
  src/vm/bytecode.cpp
  src/vm/vm.cpp
  src/runtime/types.cpp
  src/runtime/runtime.cpp
  web/html_generator.cpp
  web/mtr_parser.cpp
)

target_include_directories(mantra_lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
  target_compile_options(mantra_lib 
    PRIVATE /W4 /utf-8)
else()
  target_compile_options(mantra_lib 
    PRIVATE -Wall -Wextra)
endif()

add_executable(mantra main.cpp)
target_link_libraries(mantra 
  PRIVATE mantra_lib)

enable_testing()
add_executable(mantra_tests
  tests/test_lexer.cpp
  tests/test_vm.cpp
  tests/test_stdlib_math.cpp
  tests/test_semantic.cpp
)
target_link_libraries(mantra_tests 
  PRIVATE mantra_lib)
add_test(NAME AllTests 
  COMMAND mantra_tests)

message(STATUS 
  "MANTRA v${PROJECT_VERSION}")
message(STATUS 
  "Build: ${CMAKE_BUILD_TYPE}")
