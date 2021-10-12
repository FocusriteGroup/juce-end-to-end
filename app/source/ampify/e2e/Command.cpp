#include "Command.h"

namespace
{
ampify::e2e_testing::Command::Type typeFromString (const juce::String & string)
{
    using Type = ampify::e2e_testing::Command::Type;

    static const std::map<juce::String, Type> table = {
        {"click-component", Type::clickComponent},
        {"key-press", Type::keyPress},
        {"open-project", Type::openProject},
        {"close-project", Type::closeProject},
        {"new-project", Type::newProject},
        {"get-model", Type::getModel},
        {"get-screenshot", Type::getScreenshot},
        {"get-state", Type::getState},
        {"get-component-visibility", Type::getComponentVisibility},
        {"get-component-enablement", Type::getComponentEnablement},
        {"get-component-text", Type::getComponentText},
        {"get-focus-component", Type::getFocusComponent},
        {"get-component-count", Type::getComponentCount},
        {"grab-focus", Type::grabFocus},
        {"login", Type::login},
        {"logout", Type::logout},
        {"get-user-login-state", Type::getUserLoginState},
        {"import-file", Type::importFile},
        {"live-export", Type::liveExport},
        {"audio-export", Type::audioExport},
        {"enable-experimental-features", Type::enableExperimentalFeatures},
        {"remote-config", Type::useRemoteConfig},
        {"quit", Type::quit},
        {"register-for-events", Type::registerForEvents},
        {"invoke-menu", Type::invokeMenu},
    };

    return table.find (string) != table.end () ? table.at (string) : Type::undefined;
}

}

namespace ampify::e2e_testing
{
Command::Type Command::getType () const
{
    return _type;
}

juce::Uuid Command::getUuid () const
{
    return _uuid;
}

Command Command::fromString (const juce::String & string)
{
    juce::var root;

    if (! juce::JSON::parse (string))
        return {};

    Command command;

    command._uuid = juce::Uuid (root.getProperty ("uuid", {}));

    command._type = typeFromString (root.getProperty ("type", {}));
    if (command._type == Type::undefined)
        return {};

    command._args = root.getProperty ("args", {});

    return command;
}

bool Command::isValid () const
{
    return _type != Type::undefined && ! _uuid.isNull ();
}

juce::String Command::getArgument (const juce::String & argument) const
{
    return _args.getProperty (argument, {}).toString ();
}

juce::var Command::getArgumentAsVar (const juce::String & argument) const
{
    return _args.getProperty (argument, {});
}

}
