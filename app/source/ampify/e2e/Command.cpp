#include "Command.h"

namespace ampify::e2e
{
juce::String Command::getType () const
{
    return _type;
}

juce::Uuid Command::getUuid () const
{
    return _uuid;
}

Command Command::fromJson (const juce::String & json)
{
    const auto root = juce::JSON::parse (json);
    return root == juce::var () ? Command ()
                                : Command (root.getProperty ("type", {}),
                                           juce::Uuid (root.getProperty ("uuid", {})),
                                           root.getProperty ("args", {}));
}

bool Command::isValid () const
{
    return _type.isNotEmpty () && ! _uuid.isNull ();
}

juce::String Command::getArgument (const juce::String & argument) const
{
    return _args.getProperty (argument, {}).toString ();
}

juce::var Command::getArgumentAsVar (const juce::String & argument) const
{
    return _args.getProperty (argument, {});
}

Command::Command (juce::String type, const juce::Uuid & uuid, juce::var args)
    : _type (std::move (type))
    , _uuid (uuid)
    , _args (std::move (args))
{
}

}
