#pragma once

#include <juce_core/juce_core.h>

namespace ampify::e2e
{
class Command
{
public:
    static Command fromJson (const juce::String & json);

    [[nodiscard]] bool isValid () const;

    [[nodiscard]] juce::String getType () const;
    [[nodiscard]] juce::Uuid getUuid () const;
    [[nodiscard]] juce::String getArgument (const juce::String & argument) const;
    [[nodiscard]] juce::var getArgumentAsVar (const juce::String & argument) const;

private:
    Command () = default;
    Command (juce::String type, const juce::Uuid & uuid, juce::var args);

    juce::String _type;
    juce::Uuid _uuid = juce::Uuid::null ();
    juce::var _args;
};

}
