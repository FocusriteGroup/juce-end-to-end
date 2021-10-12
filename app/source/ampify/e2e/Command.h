#pragma once

#include <juce/JuceCore.h>

namespace ampify
{
namespace testing
{
class Command
{
public:
    enum class Type
    {
        undefined,
        clickComponent,
        keyPress,

        openProject,
        closeProject,
        newProject,

        getModel,
        getScreenshot,
        getState,

        getComponentVisibility,
        getComponentEnablement,
        getComponentText,
        getFocusComponent,
        getComponentCount,
        grabFocus,

        login,
        logout,
        getUserLoginState,

        invokeMenu,

        importFile,

        liveExport,
        audioExport,

        enableExperimentalFeatures,

        useRemoteConfig,

        quit,

        registerForEvents,
    };

    static Command fromString (const String & string);

    [[nodiscard]] bool isValid () const;

    [[nodiscard]] Type getType () const;
    [[nodiscard]] Uuid getUuid () const;
    [[nodiscard]] String getArgument (const String & argument) const;
    [[nodiscard]] var getArgumentAsVar (const String & argument) const;

private:
    Type _type = Type::undefined;
    Uuid _uuid = Uuid::null ();
    var _args;
};

}
}
