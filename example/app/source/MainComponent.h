#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent ()
    {
        setSize (400, 240);

        incrementButton.onClick = [this] { increment (); };
        decrementButton.onClick = [this] { decrement (); };
        enableButton.onClick = [this] { toggleEnableButton (); };

        addAndMakeVisible (incrementButton);
        addAndMakeVisible (decrementButton);
        addAndMakeVisible (enableButton);
        addAndMakeVisible (valueLabel);

        valueLabel.setJustificationType (juce::Justification::centred);
        valueLabel.setColour (juce::Label::textColourId, juce::Colours::black);

        updateLabel ();
    }

    void resized () override
    {
        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;

        const auto spacer = juce::FlexItem ().withHeight (8.f);
        static constexpr auto rowHeight = 30.f;

        flexBox.items = {
            juce::FlexItem (enableButton).withHeight (rowHeight),
            spacer,

            juce::FlexItem (incrementButton).withHeight (rowHeight),
            spacer,
            juce::FlexItem (decrementButton).withHeight (rowHeight),
            spacer,
            juce::FlexItem (valueLabel).withHeight (rowHeight),
        };

        flexBox.performLayout (getLocalBounds ().reduced (10));
    }

private:
    void increment ()
    {
        setValue (value + 1);
    }

    void decrement ()
    {
        setValue (value - 1);
    }

    void setValue (int newValue)
    {
        if (newValue != value)
        {
            value = newValue;
            updateLabel ();
        }
    }

    void updateLabel ()
    {
        valueLabel.setText (juce::String (value), juce::dontSendNotification);
    }

    void toggleEnableButton ()
    {
        bool willEnable = ! incrementButton.isEnabled ();
        incrementButton.setEnabled (willEnable);
        decrementButton.setEnabled (willEnable);
        enableButton.setButtonText (incrementButton.isEnabled () ? "Disable" : "Enable");
    }

    int value = 0;
    juce::TextButton incrementButton {"Increment"};
    juce::TextButton decrementButton {"Decrement"};
    juce::TextButton enableButton {"Disable"};
    juce::Label valueLabel;
};