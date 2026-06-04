cmake_minimum_required(VERSION 3.14)
project(mantra VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Try to find nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, using bundled version")
    include(FetchContent)
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(json)
endif()

# Source files
set(CORE_SOURCES
    core/main.cpp
    core/lexer.cpp
    core/parser.cpp
    core/interpreter.cpp
    core/semantic_analyzer.cpp
    core/keyword_normalizer.cpp
    core/typesystem.cpp
    core/repl.cpp
    core/module_system.cpp
    core/async_system.cpp
    core/ind_package_manager.cpp
)

set(COMPILER_SOURCES
    compiler/compiler.cpp
)

set(VM_SOURCES
    vm/vm.cpp
)

set(STDLIB_SOURCES
    stdlib/io.cpp
    stdlib/math.cpp
    stdlib/string.cpp
    stdlib/file.cpp
    stdlib/sqlite.cpp
    stdlib/network.cpp
    stdlib/http.cpp
    stdlib/socket.cpp
    stdlib/server.cpp
    stdlib/json.cpp
    stdlib/xml.cpp
    stdlib/yaml.cpp
    stdlib/zip.cpp
    stdlib/pdf.cpp
    stdlib/gui.cpp
    stdlib/browser.cpp
    stdlib/crypto.cpp
    stdlib/hash.cpp
    stdlib/process.cpp
    stdlib/ai.cpp
    stdlib/web_framework.cpp
    stdlib/testing.cpp
)

set(WEB_SOURCES
    web/mtr_parser.cpp
)

set(ALL_SOURCES
    ${CORE_SOURCES}
    ${COMPILER_SOURCES}
    ${VM_SOURCES}
    ${STDLIB_SOURCES}
    ${WEB_SOURCES}
)

# Executable
add_executable(mantra ${ALL_SOURCES})

# Include directories
target_include_directories(mantra PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/vm
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/web
    ${CMAKE_CURRENT_SOURCE_DIR}/semantic
)

# Link libraries
target_link_libraries(mantra PRIVATE
    CURL::libcurl
    Threads::Threads
    nlohmann_json::nlohmann_json
    sqlite3
    z
    ssl
    crypto
)

# Compiler options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mantra PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()

if(MSVC)
    target_compile_options(mantra PRIVATE /W4 /O2)
endif()

# Install
install(TARGETS mantra RUNTIME DESTINATION bin)
install(DIRECTORY stdlib/ DESTINATION include/mantra/stdlib FILES_MATCHING PATTERN "*.h")
install(DIRECTORY core/ DESTINATION include/mantra/core FILES_MATCHING PATTERN "*.h")

message(STATUS "MANTRA v${PROJECT_VERSION} - Configured successfully")
