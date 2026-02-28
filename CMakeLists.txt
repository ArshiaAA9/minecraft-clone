cmake_minimum_required(VERSION 3.16)
project(SunnyEngine VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # Critical for clangd
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")


find_package(SFML 3.0.2 COMPONENTS System Window Graphics Audio REQUIRED)
find_package(SunnyLog REQUIRED)
find_package(OpenGL REQUIRED)

add_library(glad include/glad/src/glad.c)
target_include_directories(glad PUBLIC include/glad/include)

# imgui: 
message(STATUS "Fetching ImGui from GitHub (this may take a while)...")
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.3
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(imgui)

# Set to build ImGui-SFML as shared to avoid forcing static SFML
set(BUILD_SHARED_LIBS ON)

# Fetch ImGui-SFML
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML ON)  # Use find_package(SFML)
set(IMGUI_SFML_IMGUI_DEMO OFF)  # Optional: Disable demo if not needed
set(SFML_STATIC_LIBRARIES OFF)  # Use shared SFML libraries
FetchContent_MakeAvailable(imgui-sfml)

# Reset BUILD_SHARED_LIBS if needed for other parts (optional)
set(BUILD_SHARED_LIBS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.c")

add_executable(bin 
    ${SOURCES}
)

target_link_libraries(bin PRIVATE
    ImGui-SFML::ImGui-SFML
    glad
    OpenGL::GL
    SunnyLog::SunnyLog
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
)

target_include_directories(bin PRIVATE
    ${SOURCE_DIR} 
    include
)
