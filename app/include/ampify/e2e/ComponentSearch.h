#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ampify::e2e
{
class ComponentSearch
{
public:
    static constexpr char testId [] = "test-id";
    static constexpr char windowId [] = "window-id";

    static juce::Component * findWithId (const juce::String & componentId, int skip = 0);
    static juce::TopLevelWindow * findWindowWithId (const juce::String & windowId = {});

    static int countChildComponents (const juce::Component & parent,
                                     const juce::String & matchingId);
};

}
