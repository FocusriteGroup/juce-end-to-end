#pragma once

#include <juce_core/juce_core.h>

namespace ampify::e2e_testing
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

    static Command fromString (const juce::String & string);

    [[nodiscard]] bool isValid () const;

    [[nodiscard]] Type getType () const;
    [[nodiscard]] juce::Uuid getUuid () const;
    [[nodiscard]] juce::String getArgument (const juce::String & argument) const;
    [[nodiscard]] juce::var getArgumentAsVar (const juce::String & argument) const;

private:
    Type _type = Type::undefined;
    juce::Uuid _uuid = juce::Uuid::null ();
    juce::var _args;
};

}
