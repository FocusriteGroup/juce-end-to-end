#include "Command.h"

#include <ampify/utils/json/JsonDocument.h>
#include <ampify/utils/json/JsonElement.h>

namespace
{
ampify::testing::Command::Type typeFromString (const String & string)
{
    using Type = ampify::testing::Command::Type;

    static const std::map<String, Type> table = {
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

namespace ampify::testing
{
Command::Type Command::getType () const
{
    return _type;
}

Uuid Command::getUuid () const
{
    return _uuid;
}

Command Command::fromString (const String & string)
{
    JsonDocument document (string);

    if (! document.parsedCorrectly ())
        return Command ();

    auto root = document.getDocumentElement ();

    Command command;

    command._uuid = Uuid (root.getStringAttribute ("uuid"));

    command._type = typeFromString (root.getStringAttribute ("type"));
    if (command._type == Type::undefined)
        return Command ();

    command._args = root.getElement ().getProperty ("args", var ());

    return command;
}

bool Command::isValid () const
{
    return _type != Type::undefined && _uuid != Uuid::null ();
}

String Command::getArgument (const String & argument) const
{
    return _args.getProperty (argument, var ()).toString ();
}

var Command::getArgumentAsVar (const String & argument) const
{
    return _args.getProperty (argument, var ());
}

}
