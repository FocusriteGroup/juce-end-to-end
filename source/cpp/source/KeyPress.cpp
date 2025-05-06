#include "KeyPress.h"

namespace focusrite::e2e
{
[[nodiscard]] static int mapKeyCode (const juce::String & code)
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

[[nodiscard]] static juce::ModifierKeys mapModifierKeys (const juce::String & modifiers)
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

}
