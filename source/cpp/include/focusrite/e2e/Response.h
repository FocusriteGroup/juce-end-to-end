#pragma once

#include <juce_core/juce_core.h>

namespace focusrite::e2e
{
class Response
{
public:
    static Response ok ();
    static Response fail (const juce::String & message);

    [[nodiscard]] Response withParameter (const juce::String & name, const juce::var & value) const;

    [[nodiscard]] Response withUuid (const juce::Uuid & uuid) const;

    [[nodiscard]] juce::String toJson () const;
    [[nodiscard]] juce::String describe () const;

    void addParameter (const juce::String & name, const juce::var & value);

private:
    explicit Response (juce::Result result);

    juce::Uuid _uuid = juce::Uuid::null ();
    juce::Result _result;
    std::map<juce::String, juce::var> _parameters;
};

}
