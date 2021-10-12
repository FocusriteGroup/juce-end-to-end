#pragma once

#include "MainComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>

class MainWindow final : public juce::DocumentWindow
{
public:
    MainWindow ()
        : juce::DocumentWindow (juce::JUCEApplication::getInstance ()->getApplicationName (),
                                juce::Colours::white,
                                juce::DocumentWindow::allButtons)
    {
        setContentNonOwned (&_mainComponent, true);
        setUsingNativeTitleBar (true);
        setResizable (true, true);
        toFront (true);
    }

    void closeButtonPressed () override
    {
        juce::JUCEApplication::quit ();
    }

private:
    MainComponent _mainComponent;
};