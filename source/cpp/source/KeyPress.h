#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace focusrite::e2e
{
juce::KeyPress constructKeyPress (const juce::String & code, const juce::String & modifiers);
}
