cmake_minimum_required(VERSION 3.22)

project(Modality VERSION 0.1.0)

# Add this line, to enable compile command export (needed for Language Server)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(cmake/cpm.cmake)

include(cmake/osx.cmake)

set(PROJECT_NAME "Modality")

CPMAddPackage(
  NAME JUCE
  GITHUB_REPOSITORY juce-framework/JUCE
  GIT_TAG 8.0.6
  VERSION 8.0.6
  SOURCE_DIR ${LIB_DIR}/juce
)

# --- Core library (everything testable) ---------------------------------
file(GLOB_RECURSE CoreSources CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
list(FILTER CoreSources EXCLUDE REGEX ".*/Source/Main\\.cpp$")

add_library(${PROJECT_NAME}Core STATIC ${CoreSources})

target_include_directories(${PROJECT_NAME}Core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(${PROJECT_NAME}Core
  PUBLIC
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

target_compile_definitions(${PROJECT_NAME}Core
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_MODAL_LOOPS_PERMITTED=1
)

# --- The app -----------------------------------------------------------
juce_add_gui_app(${PROJECT_NAME}
  PRODUCT_NAME ${PROJECT_NAME}
  COMPANY_NAME MyCompany
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    APP_NAME="${PROJECT_NAME}"
    APP_VERSION="${PROJECT_VERSION}"
)

target_sources(${PROJECT_NAME} PRIVATE Source/Main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Core)

if (MSVC)
  add_compile_options(/Wall /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- Tests -------------------------------------------------------------
option(MODALITY_BUILD_TESTS "Build unit tests" ON)

if (MODALITY_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
