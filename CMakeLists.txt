cmake_minimum_required(VERSION 3.15)
project(MantraLang VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
if(MSVC)
    # MSVC compiler flags
    add_compile_options(/W4 /WX /utf-8)
else()
    # GCC/Clang compiler flags
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -fvisibility=hidden)
    add_compile_options(-fstrict-aliasing -Wstrict-aliasing)
    add_compile_options(-Wformat=2 -Wundef -Wshadow)
endif()

# Debug and Release configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        add_compile_options(/Od /D_DEBUG)
    else()
        add_compile_options(-O0 -g -D_DEBUG)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2 /DNDEBUG)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Source files
set(CORE_SOURCES
    core/token.h
    core/lexer.h
    core/lexer.cpp
    core/ast.h
    core/parser.h
    core/parser.cpp
    core/interpreter.h
    core/interpreter.cpp
    core/repl.h
    core/repl.cpp
)

set(UNICODE_SOURCES
    unicode/unicode.h
    unicode/unicode.cpp
    unicode/script_detect.h
    unicode/script_detect.cpp
)

set(ERROR_SOURCES
    error/error.h
    error/error.cpp
)

set(STDLIB_SOURCES
    stdlib/io.h
    stdlib/io.cpp
    stdlib/math.h
    stdlib/math.cpp
    stdlib/string.h
    stdlib/string.cpp
)

# Create library
add_library(mantra_lib STATIC
    ${CORE_SOURCES}
    ${UNICODE_SOURCES}
    ${ERROR_SOURCES}
    ${STDLIB_SOURCES}
)

# Keep the library's internal headers private; the executable includes files by repo-relative paths.
target_include_directories(mantra_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/unicode
    ${CMAKE_CURRENT_SOURCE_DIR}/error
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib
)

# Create executable (if main.cpp exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    add_executable(mantra main.cpp)
    target_link_libraries(mantra PRIVATE mantra_lib)
endif()

enable_testing()
add_executable(mantra_tests tests/test_lexer.cpp)
target_link_libraries(mantra_tests PRIVATE mantra_lib)
add_test(NAME mantra_lexer_tests COMMAND mantra_tests)
add_custom_target(mantra_test
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS mantra_tests
)

# Installation targets
install(TARGETS mantra_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    core/token.h
    core/lexer.h
    DESTINATION include/mantra/core
)

install(FILES
    unicode/unicode.h
    unicode/script_detect.h
    DESTINATION include/mantra/unicode
)

install(FILES
    error/error.h
    DESTINATION include/mantra/error
)

# Print build configuration
message(STATUS "MANTRA Language Compiler")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
