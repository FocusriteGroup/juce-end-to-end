#include "DefaultCommandHandler.h"

#include "ClickableComponent.h"
#include "Command.h"
#include "ComponentSearch.h"
#include "KeyPress.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace ampify::e2e
{
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

void clickClickableComponent (ampify::e2e::ClickableComponent & clickable, int numClicks)
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
            *clickable, juce::jlimit (1, 2, command.getArgument ("num-clicks").getIntValue ()));
        return true;
    }

    return false;
}

Response clickComponent (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId == juce::String ())
        return Response::fail ("Missing component-id");

    auto skip = command.getArgument ("skip").getIntValue ();

    auto component = ComponentSearch::findWithId (componentId, skip);
    if (component == nullptr)
        return Response::fail ("Component not found: " + juce::String (componentId));

    auto handled = clickButton (*component);
    handled = handled || clickTextEditor (*component);
    handled = handled || clickClickableComponent (*component, command);

    return handled ? Response::ok ()
                   : Response::fail ("Component not clickable: " + juce::String (componentId));
}

Response keyPress (const Command & command)
{
    auto keyCode = command.getArgument ("key-code");
    if (keyCode.isEmpty ())
        return Response::fail ("Missing key-code argument");

    auto modifiers = command.getArgument ("modifiers");

    auto componentId = command.getArgument ("focus-component");

    const auto keyPress = constructKeyPress (keyCode, modifiers);

    if (componentId.isNotEmpty ())
    {
        if (auto * component = ComponentSearch::findWithId (componentId))
            component->keyPressed (keyPress);
        else
            return Response::fail ("Component not found: " + componentId);
    }
    else if (auto mainWindow = ComponentSearch::getMainWindow ())
    {
        mainWindow->getPeer ()->handleKeyPress (keyPress);
    }

    return Response::ok ();
}

Response grabFocus (const Command & command)
{
    if (command.getArgument ("require-null-focus") == "true" &&
        juce::Component::getCurrentlyFocusedComponent () != nullptr)
        return Response::ok ();

    if (auto * mainWindow = ComponentSearch::getMainWindow ())
        mainWindow->grabKeyboardFocus ();

    return Response::ok ();
}

Response getScreenshot (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");

    auto component = componentId.isEmpty () ? ComponentSearch::getMainWindow ()
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
    const auto componentId = command.getArgument ("component-id");
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    auto exists = false;
    auto showing = false;

    if (auto * component = ComponentSearch::findWithId (componentId))
    {
        exists = true;
        showing = component->isShowing ();
    }

    return Response::ok ()
        .withParameter ("exists", exists ? "true" : "false")
        .withParameter ("showing", showing ? "true" : "false");
}

Response getComponentEnablement (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    bool exists = false;
    bool enabled = false;

    if (auto * component = ComponentSearch::findWithId (componentId))
    {
        exists = true;
        enabled = component->isEnabled ();
    }

    return Response::ok ()
        .withParameter ("exists", exists ? "true" : "false")
        .withParameter ("enabled", enabled ? "true" : "false");
}

Response getComponentText (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    auto * component = ComponentSearch::findWithId (componentId);
    if (! component)
        return Response::fail ("No matching component");

    auto * textBox = dynamic_cast<juce::TextEditor *> (component);
    if (! textBox)
        return Response::fail ("Component is not a text editor");

    return Response::ok ().withParameter ("text", textBox->getText ());
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
    const auto componentId = command.getArgument ("component-id");

    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    const auto rootId = command.getArgument ("root-id");

    juce::Component * rootComponent = ComponentSearch::getMainWindow ();

    if (rootId.isNotEmpty ())
    {
        rootComponent = ComponentSearch::findWithId (rootId);
        if (rootComponent == nullptr)
            return Response::fail ("Couldn't find specified root component");
    }

    jassert (rootComponent != nullptr);

    const auto count = ComponentSearch::countChildComponents (*rootComponent, componentId);
    return Response::ok ().withParameter ("count", juce::String (count));
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

    const auto menuTitle = command.getArgument ("title");
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
