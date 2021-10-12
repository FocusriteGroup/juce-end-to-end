#pragma once

#include <juce_core/juce_core.h>

namespace ampify::e2e
{
class Response
{
public:
    static Response ok ();
    static Response fail (const juce::String & message);

    ~Response () = default;

    [[nodiscard]] Response withParameter (const juce::String & name,
                                          const juce::String & value) const;
    [[nodiscard]] Response withUuid (const juce::Uuid & uuid) const;

    juce::String toString ();

    void addParameter (const juce::String & name, const juce::String & value);

private:
    explicit Response (juce::Result result);

    juce::Uuid _uuid = juce::Uuid::null ();
    juce::Result _result;
    std::map<juce::String, juce::String> _parameters;
};

}
