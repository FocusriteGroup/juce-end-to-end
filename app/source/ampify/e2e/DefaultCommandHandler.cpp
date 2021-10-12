#include "DefaultCommandHandler.h"

#include "ClickableComponent.h"
#include "Command.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace ampify::e2e
{
int mapKeyCode (const juce::String & code)
{
    using KeyPress = juce::KeyPress;

    static const std::map<juce::String, int> values = {
        {"space", KeyPress::spaceKey},
        {"escape", KeyPress::escapeKey},
        {"return", KeyPress::returnKey},
        {"tab", KeyPress::tabKey},
        {"delete", KeyPress::deleteKey},
        {"backspace", KeyPress::backspaceKey},
        {"insert", KeyPress::insertKey},
        {"up", KeyPress::upKey},
        {"down", KeyPress::downKey},
        {"left", KeyPress::leftKey},
        {"right", KeyPress::rightKey},
        {"page-up", KeyPress::pageUpKey},
        {"page-down", KeyPress::pageDownKey},
        {"home", KeyPress::homeKey},
        {"end", KeyPress::endKey},
        {"F1", KeyPress::F1Key},
        {"F2", KeyPress::F2Key},
        {"F3", KeyPress::F3Key},
        {"F4", KeyPress::F4Key},
        {"F5", KeyPress::F5Key},
        {"F6", KeyPress::F6Key},
        {"F7", KeyPress::F7Key},
        {"F8", KeyPress::F8Key},
        {"F9", KeyPress::F9Key},
        {"F10", KeyPress::F10Key},
        {"F11", KeyPress::F11Key},
        {"F12", KeyPress::F12Key},
        {"F13", KeyPress::F13Key},
        {"F14", KeyPress::F14Key},
        {"F15", KeyPress::F15Key},
        {"F16", KeyPress::F16Key},
        {"F17", KeyPress::F17Key},
        {"F18", KeyPress::F18Key},
        {"F19", KeyPress::F19Key},
        {"F20", KeyPress::F20Key},
        {"F21", KeyPress::F21Key},
        {"F22", KeyPress::F22Key},
        {"F23", KeyPress::F23Key},
        {"F24", KeyPress::F24Key},
        {"F25", KeyPress::F25Key},
        {"F26", KeyPress::F26Key},
        {"F27", KeyPress::F27Key},
        {"F28", KeyPress::F28Key},
        {"F29", KeyPress::F29Key},
        {"F30", KeyPress::F30Key},
        {"F31", KeyPress::F31Key},
        {"F32", KeyPress::F32Key},
        {"F33", KeyPress::F33Key},
        {"F34", KeyPress::F34Key},
        {"F35", KeyPress::F35Key},
        {"num-pad-0", KeyPress::numberPad0},
        {"num-pad-1", KeyPress::numberPad1},
        {"num-pad-2", KeyPress::numberPad2},
        {"num-pad-3", KeyPress::numberPad3},
        {"num-pad-4", KeyPress::numberPad4},
        {"num-pad-5", KeyPress::numberPad5},
        {"num-pad-6", KeyPress::numberPad6},
        {"num-pad-7", KeyPress::numberPad7},
        {"num-pad-8", KeyPress::numberPad8},
        {"num-pad-9", KeyPress::numberPad9},
        {"num-pad-add", KeyPress::numberPadAdd},
        {"num-pad-subtract", KeyPress::numberPadSubtract},
        {"num-pad-multiply", KeyPress::numberPadMultiply},
        {"num-pad-divide", KeyPress::numberPadDivide},
        {"num-pad-separator", KeyPress::numberPadSeparator},
        {"num-pad-decimal-point", KeyPress::numberPadDecimalPoint},
        {"num-pad-equals", KeyPress::numberPadEquals},
        {"num-pad-delete", KeyPress::numberPadDelete},
        {"play", KeyPress::playKey},
        {"stop", KeyPress::stopKey},
        {"fast-forward", KeyPress::fastForwardKey},
        {"rewind", KeyPress::rewindKey}};

    auto it = values.find (code);
    return it != values.end () ? it->second : uint8_t (code.getCharPointer () [0]);
}

juce::ModifierKeys mapModifierKeys (const juce::String & modifiers)
{
    int modifierKeys = juce::ModifierKeys::noModifiers;
    if (modifiers.contains ("shift"))
        modifierKeys |= juce::ModifierKeys::shiftModifier;
    if (modifiers.contains ("control"))
        modifierKeys |= juce::ModifierKeys::ctrlModifier;
    if (modifiers.contains ("alt"))
        modifierKeys |= juce::ModifierKeys::altModifier;
#if JUCE_MAC
    if (modifiers.contains ("command"))
        modifierKeys |= juce::ModifierKeys::commandModifier;
#endif

    return modifierKeys;
}

juce::KeyPress constructKeyPress (const juce::String & code, const juce::String & modifiers)
{
    return {mapKeyCode (code), mapModifierKeys (modifiers), 0};
}

std::function<bool (juce::Component &)> matchComponentWithId (const juce::String & componentId)
{
    return [componentId] (auto && component) -> bool
    {
        return component.getProperties ()
                   .getWithDefault ("test-id", {})
                   .toString ()
                   .matchesWildcard (componentId, false) &&
               component.isVisible () && component.isShowing ();
    };
}

int countChildComponents (juce::Component & root,
                          const std::function<bool (juce::Component &)> & predicate)
{
    int count = 0;
    for (int childIndex = 0; childIndex < root.getNumChildComponents (); ++childIndex)
    {
        if (auto * child = root.getChildComponent (childIndex))
        {
            if (predicate (*child))
                ++count;

            count += countChildComponents (*child, predicate);
        }
    }

    return count;
}

juce::Component * matchChildComponent (juce::Component & component,
                                       const std::function<bool (juce::Component &)> & predicate,
                                       int & skip)
{
    for (int childIndex = 0; childIndex < component.getNumChildComponents (); ++childIndex)
    {
        auto * child = component.getChildComponent (childIndex);
        if (! child)
            continue;

        if (predicate (*child))
        {
            if (skip == 0)
                return child;
            else
                --skip;
        }
    }

    for (int childIndex = 0; childIndex < component.getNumChildComponents (); ++childIndex)
    {
        auto * child = component.getChildComponent (childIndex);
        if (auto * foundComponent = matchChildComponent (*child, predicate, skip))
            return foundComponent;
    }

    return nullptr;
}

juce::Component * findChildComponent (juce::Component & component,
                                      const std::function<bool (juce::Component &)> & predicate,
                                      int skip = 0)
{
    return matchChildComponent (component, predicate, skip);
}

std::vector<juce::TopLevelWindow *> getTopLevelWindows ()
{
    std::vector<juce::TopLevelWindow *> windows;

    for (int windowIndex = 0; windowIndex < juce::TopLevelWindow::getNumTopLevelWindows ();
         ++windowIndex)
        if (auto window = juce::TopLevelWindow::getTopLevelWindow (windowIndex))
            windows.push_back (window);

    return windows;
}

juce::TopLevelWindow * getMainWindow ()
{
    auto windows = getTopLevelWindows ();
    if (windows.empty ())
        return nullptr;

    auto it = std::find_if (
        windows.begin (), windows.end (), [] (auto && window) { return window->isVisible (); });
    return it != windows.end () ? *it : nullptr;
}

juce::Component * findComponent (const std::function<bool (juce::Component &)> & predicate,
                                 int skip = 0)
{
    for (auto & window : getTopLevelWindows ())
        if (auto component = findChildComponent (*window, predicate, skip))
            return component;

    return nullptr;
}

juce::Component * findComponentWithId (const juce::String & componentId, int skip = 0)
{
    return findComponent (matchComponentWithId (componentId), skip);
}

void handleButtonClick (juce::Button & button)
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

void handleClickableComponentClick (ampify::e2e::ClickableComponent & clickable, int numClicks)
{
    if (numClicks == 1)
        clickable.performClick ();

    if (numClicks == 2)
        clickable.performDoubleClick ();
}

juce::String snapshotAndEncodeComponent (juce::Component & component)
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

Response handleClickComponent (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId == juce::String ())
        return Response::fail ("Missing component-id");

    auto skip = command.getArgument ("skip").getIntValue ();

    auto component = findComponentWithId (componentId, skip);
    if (component == nullptr)
        return Response::fail ("Component not found: " + juce::String (componentId));

    if (! component->isShowing ())
        return Response::fail ("Component " + componentId + " is not visible");

    if (auto * button = dynamic_cast<juce::Button *> (component))
    {
        handleButtonClick (*button);
        return Response::ok ();
    }

    if (auto * textBox = dynamic_cast<juce::TextEditor *> (component))
    {
        textBox->grabKeyboardFocus ();
        return Response::ok ();
    }

    if (auto * clickable = dynamic_cast<ClickableComponent *> (component))
    {
        handleClickableComponentClick (
            *clickable, juce::jlimit (1, 2, command.getArgument ("num-clicks").getIntValue ()));
        return Response::ok ();
    }

    return Response::fail ("Component not clickable: " + juce::String (componentId));
}

Response handleKeyPress (const Command & command)
{
    auto keyCode = command.getArgument ("key-code");
    if (keyCode.isEmpty ())
        return Response::fail ("Missing key-code argument");

    auto modifiers = command.getArgument ("modifiers");

    auto componentId = command.getArgument ("focus-component");

    const auto keyPress = constructKeyPress (keyCode, modifiers);

    if (componentId.isNotEmpty ())
    {
        if (auto * component = findComponentWithId (componentId))
            component->keyPressed (keyPress);
        else
            return Response::fail ("Component not found: " + componentId);
    }
    else if (auto mainWindow = getMainWindow ())
    {
        mainWindow->getPeer ()->handleKeyPress (keyPress);
    }

    return Response::ok ();
}

Response handleGrabFocus (const Command & command)
{
    if (command.getArgument ("require-null-focus") == "true" &&
        juce::Component::getCurrentlyFocusedComponent () != nullptr)
        return Response::ok ();

    if (auto * mainWindow = getMainWindow ())
        mainWindow->grabKeyboardFocus ();

    return Response::ok ();
}

Response handleGetScreenshot (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");

    auto component = componentId.isEmpty () ? getMainWindow () : findComponentWithId (componentId);
    if (component == nullptr)
        return Response::fail ("Component not found: " + juce::String (componentId));

    auto image = snapshotAndEncodeComponent (*component);
    if (image.isEmpty ())
        return Response::fail ("Failed to snapshot component");

    return Response (Response::ok ()).withParameter ("image", image);
}

Response handleGetComponentVisibility (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    bool exists = false;
    bool showing = false;

    if (auto * component = findComponentWithId (componentId))
    {
        exists = true;
        showing = component->isShowing ();
    }

    return Response (Response::ok ())
        .withParameter ("exists", exists ? "true" : "false")
        .withParameter ("showing", showing ? "true" : "false");
}

Response handleGetComponentEnablement (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    bool exists = false;
    bool enabled = false;

    if (auto * component = findComponentWithId (componentId))
    {
        exists = true;
        enabled = component->isEnabled ();
    }

    return Response::ok ()
        .withParameter ("exists", exists ? "true" : "false")
        .withParameter ("enabled", enabled ? "true" : "false");
}

Response handleGetComponentText (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");
    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    auto * component = findComponentWithId (componentId);
    if (! component)
        return Response::fail ("No matching component");

    auto * textBox = dynamic_cast<juce::TextEditor *> (component);
    if (! textBox)
        return Response::fail ("Component is not a text editor");

    return Response::ok ().withParameter ("text", textBox->getText ());
}

Response handleGetFocusComponent (const Command & command)
{
    juce::ignoreUnused (command);

    juce::String testId;

    if (auto * focusComponent = juce::Component::getCurrentlyFocusedComponent ())
        testId = focusComponent->getProperties ().getWithDefault ("test-id", {}).toString ();

    return Response::ok ().withParameter ("component-id", testId);
}

Response handleCountComponents (const Command & command)
{
    const auto componentId = command.getArgument ("component-id");

    if (componentId.isEmpty ())
        return Response::fail ("Missing component-id");

    const auto rootId = command.getArgument ("root-id");

    juce::Component * rootComponent = getMainWindow ();

    if (rootId.isNotEmpty ())
    {
        rootComponent = findComponentWithId (rootId);
        if (rootComponent == nullptr)
            return Response::fail ("Couldn't find specified root component");
    }

    jassert (rootComponent != nullptr);

    const auto count = countChildComponents (*rootComponent, matchComponentWithId (componentId));
    return Response::ok ().withParameter ("count", juce::String (count));
}

Response handleQuit (const Command & command)
{
    juce::ignoreUnused (command);
    return Response::ok ();
}

Response handleInvokeMenu (const Command & command)
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
            {"click-component", [&] (auto && command) { return handleClickComponent (command); }},
            {"key-press", [&] (auto && command) { return handleKeyPress (command); }},
            {"get-screenshot", [&] (auto && command) { return handleGetScreenshot (command); }},
            {"get-component-visibility",
             [&] (auto && command) { return handleGetComponentVisibility (command); }},
            {"get-component-enablement",
             [&] (auto && command) { return handleGetComponentEnablement (command); }},
            {"get-component-text",
             [&] (auto && command) { return handleGetComponentText (command); }},
            {"get-focus-component",
             [&] (auto && command) { return handleGetFocusComponent (command); }},
            {"get-component-count",
             [&] (auto && command) { return handleCountComponents (command); }},
            {"grab-focus", [&] (auto && command) { return handleGrabFocus (command); }},
            {"quit", [&] (auto && command) { return handleQuit (command); }},
            {"invoke-menu", [&] (auto && command) { return handleInvokeMenu (command); }},
        };

    auto it = commandHandlers.find (command.getType ());

    if (it == commandHandlers.end ())
        return std::nullopt;

    return it->second (command);
}
}
