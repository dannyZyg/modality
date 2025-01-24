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

juce_add_gui_app(${PROJECT_NAME}
  PRODUCT_NAME ${PROJECT_NAME}
  COMPANY_NAME MyCompany
)

juce_generate_juce_header(${PROJECT_NAME})

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Add source files to the app target
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

# Include the Source directory
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

if (MSVC)
  add_compile_options(/Wall /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
