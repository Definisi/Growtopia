cmake_minimum_required(VERSION 3.16)

# vcpkg integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Set vcpkg toolchain file if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Project configuration
project(GrowtopiaBot 
    VERSION 1.0.0
    DESCRIPTION "Advanced Growtopia Bot with HTTP API"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/w)
    add_compile_options(/W4 /WX- /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Debug/Release specific flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
    if(MSVC)
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(-g -O0)
    endif()
else()
    add_compile_definitions(RELEASE_BUILD NDEBUG)
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/lua
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/md5
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/proton
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/sha256
)

# Find required packages
find_package(Threads REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

# Source files
file(GLOB_RECURSE CORE_SOURCES
    "src/core/*.cpp"
    "src/core/*.hpp"
)

file(GLOB_RECURSE API_SOURCES
    "src/api/*.cpp"
    "src/api/*.hpp"
)

file(GLOB_RECURSE CLIENT_SOURCES
    "src/client/*.cpp"
    "src/client/*.hpp"
)

file(GLOB_RECURSE SERVER_SOURCES
    "src/server/*.cpp"
    "src/server/*.hpp"
)

file(GLOB_RECURSE UTILS_SOURCES
    "src/utils/*.cpp"
    "src/utils/*.hpp"
)

# Remove ImGui-related files from utils sources
list(FILTER UTILS_SOURCES EXCLUDE REGEX ".*imgui.*")
list(FILTER UTILS_SOURCES EXCLUDE REGEX ".*gui.*")
list(FILTER UTILS_SOURCES EXCLUDE REGEX ".*blur.*")

file(GLOB_RECURSE ITEM_SOURCES
    "src/item/*.cpp"
    "src/item/*.hpp"
)

file(GLOB_RECURSE PLAYER_SOURCES
    "src/player/*.cpp"
    "src/player/*.hpp"
)

file(GLOB_RECURSE WORLD_SOURCES
    "src/world/*.cpp"
    "src/world/*.hpp"
)

file(GLOB_RECURSE EVENTS_SOURCES
    "src/events/*.cpp"
    "src/events/*.hpp"
)

file(GLOB_RECURSE LAPI_SOURCES
    "src/lapi/*.cpp"
    "src/lapi/*.hpp"
)

# GUI sources removed - no longer using ImGui/DirectX

# Vendor sources
file(GLOB_RECURSE VENDOR_SOURCES
    "vendor/lua/*.cpp"
    "vendor/sha256/*.cpp"
)

# Add MD5 source file separately as C file
set(MD5_SOURCE "vendor/md5/simple_md5.c")
set_source_files_properties(${MD5_SOURCE} PROPERTIES LANGUAGE C)

# Main executable
add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    ${API_SOURCES}
    ${CLIENT_SOURCES}
    ${SERVER_SOURCES}
    ${UTILS_SOURCES}
    ${ITEM_SOURCES}
    ${PLAYER_SOURCES}
    ${WORLD_SOURCES}
    ${EVENTS_SOURCES}
    ${LAPI_SOURCES}
    ${VENDOR_SOURCES}
    ${MD5_SOURCE}
    src/main.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    cpprestsdk::cpprest
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        ws2_32
        winmm
    )
endif()

# Copy required files to output directory (Windows)
if(WIN32)
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/items.dat"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/main.lua"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Create logs directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/logs
)

# Install configuration
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(FILES
    items.dat
    main.lua
    DESTINATION bin
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "GrowtopiaBot")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "GrowtopiaBot Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

include(CPack)

# Enable testing
enable_testing()

# Add custom targets for code formatting and linting
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i -style=file
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.hpp
        COMMENT "Formatting code with clang-format"
    )
endif()

find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    add_custom_target(lint
        COMMAND ${CLANG_TIDY}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp
        -- -I${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Linting code with clang-tidy"
    )
endif()

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Project version: ${PROJECT_VERSION}")