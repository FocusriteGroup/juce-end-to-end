#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ampify::e2e
{
class ComponentSearch
{
public:
    static constexpr char testId [] = "test-id";

    static juce::Component * findWithId (const juce::String & componentId, int skip = 0);
    static juce::TopLevelWindow * getMainWindow ();
    static int countChildComponents (const juce::Component & parent,
                                     const juce::String & matchingId);
};

}
