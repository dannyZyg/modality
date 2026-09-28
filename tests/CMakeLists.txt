CPMAddPackage(
  NAME Catch2
  GITHUB_REPOSITORY catchorg/Catch2
  VERSION 3.8.1
  GIT_TAG v3.8.1
  OPTIONS
    "CATCH_INSTALL_DOCS OFF"
    "CATCH_INSTALL_EXTRAS OFF"
)

# Provides catch_discover_tests()
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(ModalityTests
  test_example.cpp
)

target_link_libraries(ModalityTests
  PRIVATE
    ModalityCore
    Catch2::Catch2WithMain
)

catch_discover_tests(ModalityTests)

