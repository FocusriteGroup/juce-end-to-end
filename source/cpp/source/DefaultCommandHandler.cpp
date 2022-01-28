#include "DefaultCommandHandler.h"

#include "KeyPress.h"

#include <focusrite/e2e/ClickableComponent.h>
#include <focusrite/e2e/Command.h>
#include <focusrite/e2e/ComponentSearch.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace focusrite::e2e
{
enum class CommandArgument
{
    componentId,
    focusComponent,
    keyCode,
    modifiers,
    numClicks,
    rootId,
    skip,
    title,
    windowId,
};

juce::StringRef toString (CommandArgument argument)
{
    switch (argument)
    {
        case CommandArgument::componentId:
            return "component-id";
        case CommandArgument::focusComponent:
            return "focus-component";
        case CommandArgument::keyCode:
            return "key-code";
        case CommandArgument::modifiers:
            return "modifiers";
        case CommandArgument::numClicks:
            return "num-clicks";
        case CommandArgument::rootId:
            return "root-id";
        case CommandArgument::skip:
            return "skip";
        case CommandArgument::title:
            return "title";
        case CommandArgument::windowId:
            return "window-id";
    }

    jassertfalse;
    return {};
}

void clickButton (juce::Button & button)
{
    if (button.onClick != nullptr)
    {
        if (button.getClickingTogglesState ())
            button.setToggleState (! button.getToggleState (),
                                   juce::NotificationType::dontSendNotification);

        button.onClick ();
    }
    else
    {
        button.triggerClick ();
    }
}

void clickClickableComponent (focusrite::e2e::ClickableComponent & clickable, int numClicks)
{
    if (numClicks == 1)
        clickable.performClick ();

    if (numClicks == 2)
        clickable.performDoubleClick ();
}

juce::String snapshotComponent (juce::Component & component)
{
    auto image = component.createComponentSnapshot (component.getLocalBounds ());

    if (image.isNull ())
        return {};

    juce::MemoryOutputStream rawStream;

    juce::PNGImageFormat imageFormat;
    if (! imageFormat.writeImageToStream (image, rawStream))
        return {};

    return juce::Base64::toBase64 (rawStream.getData (), rawStream.getDataSize ());
}

bool clickButton (juce::Component & component)
{
    if (auto * button = dynamic_cast<juce::Button *> (&component))
    {
        clickButton (*button);
        return true;
    }

    return false;
}

bool clickTextEditor (juce::Component & component)
{
    if (auto * textBox = dynamic_cast<juce::TextEditor *> (&component))
    {
        textBox->grabKeyboardFocus ();
        return true;
    }

    return false;
}

bool clickClickableComponent (juce::Component & component, const Command & command)
{
    if (auto * clickable = dynamic_cast<ClickableComponent *> (&component))
    {
        clickClickableComponent (
            *clickable,
            juce::jlimit (
                1, 2, command.getArgument (toString (CommandArgument::numClicks)).getIntValue ()));
        return true;
    }

    return false;
}

Response clickComponent (const Command & command)
{
    const auto componentId = command.getArgument (toString (CommandArgument::componentId));
    if (componentId == juce::String ())
        return Response::fail ("Missing component-id");

    auto skip = command.getArgument (toString (CommandArgument::skip)).getIntValue ();

    auto component = ComponentSearch::findWithId (componentId, skip);
    if (component == nullptr)
        return Response::fail ("Component not found: " + juce::String (componentId));

    auto handled = clickButton (*component);
    handled = handled || clickTextEditor (*component);
    handled = handled || clickClickableComponent (*component, command);

    return handled ? Response::ok ()
                   : Response::fail ("Component not clickable: " + juce::String (componentId));
}

Response keyPressOnComponent (const juce::String & componentId, const juce::KeyPress & keyPress)
{
    auto * component = ComponentSearch::findWithId (componentId);
    if (component == nullptr)
        return Response::fail ("Component not found for key press: " + componentId);

    component->keyPressed (keyPress);
    return Response::ok ();
}

Response keyPressOnWindow (const juce::String & windowId, const juce::KeyPress & keyPress)
{
    auto window = ComponentSearch::findWindowWithId (windowId);
    if (! window)
        return Response::fail ("Couldn't find window");

    auto peer = window->getPeer ();
    if (! peer)
        return Response::fail ("Window doesn't have peer");

    peer->handleKeyPress (keyPress);
    return Response::ok ();
}

Response keyPress (const Command & command)
{
    auto keyCode = command.getArgument (toString (CommandArgument::keyCode));
    if (keyCode.isEmpty ())
        return Response::fail ("Missing key-code argument");

    auto modifiers = command.getArgument (toString (CommandArgument::modifiers));
    auto componentId = command.getArgument (toString (CommandArgument::focusComponent));
    auto windowId = command.getArgument (toString (CommandArgument::windowId));

    const auto keyPress = constructKeyPress (keyCode, modifiers);

    if (componentId.isNotEmpty ())
        return keyPressOnComponent (componentId, keyPress);

    return keyPressOnWindow (windowId, keyPress);
}

Response grabFocus (const Command & command)
{
    if (auto * window = ComponentSearch::findWindowWithId (
            command.getArgument (toString (CommandArgument::windowId))))
        window->grabKeyboardFocus ();

    return Response::ok ();
}

Response getScreenshot (const Command & command)
{
    const auto componentId = command.getArgument (toString (CommandArgument::componentId));
    const auto windowId = command.getArgument (toString (CommandArgument::windowId));

    auto component = componentId.isEmpty () ? ComponentSearch::findWindowWithId (windowId)
                                            : ComponentSearch::findWithId (componentId);

    if (component == nullptr)
        return Response::fail ("Component not found: " + juce::String (componentId));

    auto image = snapshotComponent (*component);
    if (image.isEmpty ())
        return Response::fail ("Failed to snapshot component");

    return Response::ok ().withParameter ("image", image);
}

Response getComponentVisibility (const Command & command)
{
    const auto componentId = command.getArgument (toString (CommandArgument::componentId));
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    auto exists = false;
    auto showing = false;

    if (auto * component = ComponentSearch::findWithId (componentId))
    {
        exists = true;
        showing = component->isShowing ();
    }

    return Response::ok ().withParameter ("exists", exists).withParameter ("showing", showing);
}

Response getComponentEnablement (const Command & command)
{
    const auto componentId = command.getArgument (toString (CommandArgument::componentId));
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    bool exists = false;
    bool enabled = false;

    if (auto * component = ComponentSearch::findWithId (componentId))
    {
        exists = true;
        enabled = component->isEnabled ();
    }

    return Response::ok ().withParameter ("exists", exists).withParameter ("enabled", enabled);
}

Response getComponentText (const Command & command)
{
    const auto componentId = command.getArgument (toString (CommandArgument::componentId));
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    auto * component = ComponentSearch::findWithId (componentId);
    if (! component)
        return Response::fail ("No matching component");

    if (auto * textBox = dynamic_cast<const juce::TextEditor *> (component))
        return Response::ok ().withParameter ("text", textBox->getText ());

    if (auto * label = dynamic_cast<const juce::Label *> (component))
        return Response::ok ().withParameter ("text", label->getText ());

    if (auto * button = dynamic_cast<const juce::Button *> (component))
        return Response::ok ().withParameter ("text", button->getButtonText ());

    return Response::fail ("Component doesn't have text");
}

Response getFocusComponent (const Command & command)
{
    juce::ignoreUnused (command);

    juce::String testId;

    if (auto * focusComponent = juce::Component::getCurrentlyFocusedComponent ())
        testId = focusComponent->getProperties ().getWithDefault ("test-id", {}).toString ();

    return Response::ok ().withParameter ("component-id", testId);
}

Response countComponents (const Command & command)
{
    const auto componentId = command.getArgument (toString (CommandArgument::componentId));

    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    const auto rootId = command.getArgument (toString (CommandArgument::rootId));
    const auto windowId = command.getArgument (toString (CommandArgument::windowId));

    juce::Component * rootComponent = ComponentSearch::findWindowWithId (windowId);

    if (rootId.isNotEmpty ())
    {
        rootComponent = ComponentSearch::findWithId (rootId);
        if (rootComponent == nullptr)
            return Response::fail ("Couldn't find specified root component");
    }

    jassert (rootComponent != nullptr);

    return Response::ok ().withParameter (
        "count", ComponentSearch::countChildComponents (*rootComponent, componentId));
}

Response quit (const Command & command)
{
    juce::ignoreUnused (command);
    return Response::ok ();
}

Response invokeMenu (const Command & command)
{
    auto * application = juce::JUCEApplication::getInstance ();
    if (! application)
        return Response::fail ("Invalid application");

    const auto menuTitle = command.getArgument (toString (CommandArgument::title));
    if (menuTitle.isEmpty ())
        return Response::fail ("Missing menu title");

    juce::Array<juce::CommandID> commands;
    application->getAllCommands (commands);

    for (auto commandID : commands)
    {
        juce::ApplicationCommandInfo info (commandID);
        application->getCommandInfo (commandID, info);

        if (info.shortName != menuTitle)
            continue;

        application->invoke (juce::ApplicationCommandTarget::InvocationInfo (commandID), false);
        return Response::ok ();
    }

    return Response::fail ("Not handled");
}

std::optional<Response> DefaultCommandHandler::process (const Command & command)
{
    static const std::map<juce::String, std::function<Response (const Command &)>> commandHandlers =
        {
            {"click-component", [&] (auto && command) { return clickComponent (command); }},
            {"key-press", [&] (auto && command) { return keyPress (command); }},
            {"get-screenshot", [&] (auto && command) { return getScreenshot (command); }},
            {"get-component-visibility",
             [&] (auto && command) { return getComponentVisibility (command); }},
            {"get-component-enablement",
             [&] (auto && command) { return getComponentEnablement (command); }},
            {"get-component-text", [&] (auto && command) { return getComponentText (command); }},
            {"get-focus-component", [&] (auto && command) { return getFocusComponent (command); }},
            {"get-component-count", [&] (auto && command) { return countComponents (command); }},
            {"grab-focus", [&] (auto && command) { return grabFocus (command); }},
            {"quit", [&] (auto && command) { return quit (command); }},
            {"invoke-menu", [&] (auto && command) { return invokeMenu (command); }},
        };

    auto it = commandHandlers.find (command.getType ());

    if (it == commandHandlers.end ())
        return std::nullopt;

    return it->second (command);
}
}
