#pragma once

#include <juce_core/juce_core.h>

namespace ampify::e2e_testing
{
class Response
{
public:
    Response (const juce::Uuid & uuid, const juce::Result & result);
    ~Response () = default;

    [[nodiscard]] Response withParameter (const juce::String & name, const juce::String & value) const;

    juce::String toString ();

    void addParameter (const juce::String & name, const juce::String & value);

private:
    juce::Uuid _uuid;
    juce::Result _result;
    std::map<juce::String, juce::String> _parameters;
};

}
