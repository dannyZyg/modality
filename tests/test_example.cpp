#include <catch2/catch_test_macros.hpp>
#include <juce_core/juce_core.h>

TEST_CASE ("juce string basics", "[juce]")
{
    juce::String s ("hello");
    REQUIRE (s.length() == 5);
    REQUIRE (s.toUpperCase() == "HELLO");
}
