#pragma once

#include <juce_core/juce_core.h>
#include <map>

namespace ampify::e2e
{
class Event
{
public:
    explicit Event (juce::String name);
    Event (const Event & other) = default;
    ~Event () = default;

    [[nodiscard]] Event withParameter (const juce::String & name, const juce::String & value) const;

    [[nodiscard]] juce::String toJson () const;

    void addParameter (const juce::String & name, const juce::var & value);

private:
    juce::String _name;
    std::map<juce::String, juce::var> _parameters;
};

}
