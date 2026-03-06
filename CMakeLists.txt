cmake_minimum_required(VERSION 3.16)
project(MinecraftClone VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # Critical for clangd
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")


find_package(SFML 3.0.2 COMPONENTS System Window Graphics Audio REQUIRED)
find_package(SunnyLog REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Catch2 3 REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${Catch2_DIR})

add_library(glad include/glad/src/glad.c)
target_include_directories(glad PUBLIC include/glad/include)

# ==================== ENGINE LIBRARY ====================
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
)
list(REMOVE_ITEM ENGINE_SOURCES "${SOURCE_DIR}/main.cpp")

add_library(MinecraftClone STATIC ${ENGINE_SOURCES})

target_include_directories(MinecraftClone PUBLIC
    ${SOURCE_DIR}
    include
)

target_link_libraries(MinecraftClone PUBLIC
    glad
    SunnyLog::SunnyLog
    OpenGL::GL
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
)

# ==================== MAIN EXECUTABLE ====================
add_executable(bin ${SOURCE_DIR}/main.cpp)
target_link_libraries(bin PRIVATE MinecraftClone)

# ==================== TESTS ====================
add_executable(tests
    ${CMAKE_SOURCE_DIR}/tests/test.cpp
)

target_link_libraries(tests PRIVATE
    MinecraftClone
    Catch2::Catch2WithMain
)

target_include_directories(tests PRIVATE
    ${SOURCE_DIR}
    include
)

# Auto-discover tests with ctest
include(CTest)
include(Catch)
catch_discover_tests(tests)
