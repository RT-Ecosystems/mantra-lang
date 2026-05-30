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

# Find SQLite3
find_package(OpenSSL REQUIRED)
find_package(SQLite3)
if(NOT SQLite3_FOUND)
  message(FATAL_ERROR "SQLite3 not found. Please install libsqlite3-dev")
endif()

add_library(mantra_lib STATIC
  core/lexer.cpp
  core/parser.cpp
  core/interpreter.cpp
  core/repl.cpp
  core/keyword_normalizer.cpp
  core/typesystem.cpp
  error/error.cpp
  stdlib/io.cpp
  stdlib/math.cpp
  stdlib/string.cpp
    stdlib/ai.cpp
    stdlib/file.cpp
    stdlib/json.cpp
    stdlib/csv.cpp
    stdlib/datetime.cpp
    stdlib/http.cpp
    stdlib/random.cpp
    stdlib/os.cpp
    stdlib/base64.cpp
    stdlib/regex.cpp
    stdlib/uuid.cpp
    stdlib/tempfile.cpp
    stdlib/sqlite.cpp
    stdlib/zip.cpp
    stdlib/compress.cpp
    stdlib/xml.cpp
    stdlib/yaml.cpp
    stdlib/ini.cpp
    stdlib/process.cpp
    stdlib/socket.cpp
    stdlib/crypto.cpp
    stdlib/image.cpp
    stdlib/audio.cpp
    stdlib/hash.cpp
    stdlib/gui.cpp
    stdlib/chart.cpp
    stdlib/pdf.cpp
    stdlib/email.cpp
    stdlib/logging.cpp
    stdlib/serial.cpp
    stdlib/config.cpp
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
  ${SQLite3_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
)

# Link SQLite3 and OpenSSL
target_link_libraries(mantra_lib 
  PUBLIC ${SQLite3_LIBRARIES} ${OPENSSL_LIBRARIES})

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
  tests/test_typesystem.cpp
)
target_link_libraries(mantra_tests 
  PRIVATE mantra_lib)
add_test(NAME AllTests 
  COMMAND mantra_tests)

# Separate test executable for KeywordNormalizer and TokenClass
add_executable(test_keyword_normalizer
  tests/test_keyword_normalizer.cpp
)
target_link_libraries(test_keyword_normalizer 
  PRIVATE mantra_lib)
add_test(NAME KeywordNormalizerTests 
  COMMAND test_keyword_normalizer)

message(STATUS 
  "MANTRA v${PROJECT_VERSION}")
message(STATUS 
  "Build: ${CMAKE_BUILD_TYPE}")


