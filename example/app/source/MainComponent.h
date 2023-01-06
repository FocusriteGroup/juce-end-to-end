#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent ()
    {
        static constexpr auto width = 400;
        static constexpr auto height = 240;
        setSize (width, height);

        _incrementButton.onClick = [this] { increment (); };
        _decrementButton.onClick = [this] { decrement (); };
        _enableButton.onClick = [this] { toggleEnableButton (); };

        addAndMakeVisible (_incrementButton);
        addAndMakeVisible (_decrementButton);
        addAndMakeVisible (_enableButton);
        addAndMakeVisible (_valueLabel);
        addAndMakeVisible (_slider);
        addAndMakeVisible (_textEditor);

        _valueLabel.setJustificationType (juce::Justification::centred);
        _valueLabel.setColour (juce::Label::textColourId, juce::Colours::black);

        updateLabel ();

        _incrementButton.setComponentID ("increment-button");
        _decrementButton.setComponentID ("decrement-button");
        _enableButton.setComponentID ("enable-button");
        _valueLabel.setComponentID ("value-label");
        _slider.setComponentID ("slider");
        _textEditor.setComponentID ("text-editor");

        _textEditor.onTextChange = [this]
        {
            const auto text = _textEditor.getText ();
            setValue (text.getIntValue ());
        };

        _slider.onValueChange = [this] { setValue (static_cast<int> (_slider.getValue ())); };

        _incrementButton.setTitle ("Increment button title");
        _incrementButton.setDescription ("Increment button description");
        _incrementButton.setHelpText ("Increment button help text");
        _incrementButton.setTooltip ("Increment button tool tip");

        _decrementButton.setAccessible (false);
    }

    void resized () override
    {
        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;

        const auto spacer = juce::FlexItem ().withHeight (8.f);
        static constexpr auto rowHeight = 30.f;

        flexBox.items = {
            juce::FlexItem (_enableButton).withHeight (rowHeight),
            spacer,
            juce::FlexItem (_incrementButton).withHeight (rowHeight),
            spacer,
            juce::FlexItem (_decrementButton).withHeight (rowHeight),
            spacer,
            juce::FlexItem (_slider).withHeight (rowHeight),
            spacer,
            juce::FlexItem (_textEditor).withHeight (rowHeight),
            spacer,
            juce::FlexItem (_valueLabel).withHeight (rowHeight),
        };

        static constexpr auto margin = 10;
        flexBox.performLayout (getLocalBounds ().reduced (margin));
    }

private:
    void increment ()
    {
        setValue (_value + 1);
    }

    void decrement ()
    {
        setValue (_value - 1);
    }

    void setValue (int newValue)
    {
        if (newValue != _value)
        {
            _value = newValue;
            _slider.setValue (_value, juce::NotificationType::dontSendNotification);
            updateLabel ();
        }
    }

    void updateLabel ()
    {
        _valueLabel.setText (juce::String (_value), juce::dontSendNotification);
    }

    void toggleEnableButton ()
    {
        auto willEnable = ! _incrementButton.isEnabled ();
        _incrementButton.setEnabled (willEnable);
        _decrementButton.setEnabled (willEnable);
        _slider.setEnabled (willEnable);
        _textEditor.setEnabled (willEnable);
        _enableButton.setButtonText (_incrementButton.isEnabled () ? "Disable" : "Enable");
    }

    int _value = 0;
    juce::TextButton _incrementButton {"Increment"};
    juce::TextButton _decrementButton {"Decrement"};
    juce::TextButton _enableButton {"Disable"};
    juce::Label _valueLabel;
    juce::Slider _slider {juce::Slider::LinearHorizontal, juce::Slider::NoTextBox};
    juce::TextEditor _textEditor;
};
