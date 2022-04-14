#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent ()
    {
        setSize (400, 240);

        _incrementButton.onClick = [this] { increment (); };
        _decrementButton.onClick = [this] { decrement (); };
        _enableButton.onClick = [this] { toggleEnableButton (); };

        addAndMakeVisible (_incrementButton);
        addAndMakeVisible (_decrementButton);
        addAndMakeVisible (_enableButton);
        addAndMakeVisible (_valueLabel);
        addAndMakeVisible (_slider);

        _valueLabel.setJustificationType (juce::Justification::centred);
        _valueLabel.setColour (juce::Label::textColourId, juce::Colours::black);

        updateLabel ();

        _incrementButton.setComponentID ("increment-button");
        _decrementButton.setComponentID ("decrement-button");
        _enableButton.setComponentID ("enable-button");
        _valueLabel.setComponentID ("value-label");
        _slider.setComponentID ("slider");

        _slider.onValueChange = [this] { setValue (_slider.getValue ()); };
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
            juce::FlexItem (_valueLabel).withHeight (rowHeight),
            spacer,
            juce::FlexItem (_slider).withHeight (rowHeight),
        };

        flexBox.performLayout (getLocalBounds ().reduced (10));
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
        _enableButton.setButtonText (_incrementButton.isEnabled () ? "Disable" : "Enable");
    }

    int _value = 0;
    juce::TextButton _incrementButton {"Increment"};
    juce::TextButton _decrementButton {"Decrement"};
    juce::TextButton _enableButton {"Disable"};
    juce::Label _valueLabel;
    juce::Slider _slider {juce::Slider::LinearHorizontal, juce::Slider::NoTextBox};
};