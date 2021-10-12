#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent ()
    {
        setSize (800, 600);
    }
};