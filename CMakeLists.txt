cmake_minimum_required(VERSION 3.16)

# ✅ Generate compile_commands.json (clangd / Zed use this)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Default to Debug builds unless explicitly set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(hack-language-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# nlohmann/json submodule
add_subdirectory(external/nlohmann_json)

# Collect all source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(hack-language-server ${SOURCES})

# Add include directories for headers
target_include_directories(hack-language-server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link against nlohmann/json
target_link_libraries(hack-language-server PRIVATE nlohmann_json::nlohmann_json)

# Common warnings
target_compile_options(hack-language-server PRIVATE -Wall -Wextra -Wpedantic)

# Debug vs Release flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_options(hack-language-server PRIVATE -g -O0)
  message(STATUS "Building in Debug mode with debug symbols (-g -O0)")
else()
  target_compile_options(hack-language-server PRIVATE -O2)
endif()
