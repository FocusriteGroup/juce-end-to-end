#pragma once

#include "MainWindow.h"

#include <focusrite/e2e/TestCentre.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Application final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName () override // NOLINT
    {
        return "E2E Test Application";
    }

    const juce::String getApplicationVersion () override // NOLINT
    {
        return "0.0.0";
    }

    void initialise ([[maybe_unused]] const juce::String & commandLineArguments) override
    {
        _mainWindow.setVisible (true);
    }

    void shutdown () override
    {
        _mainWindow.setVisible (false);
    }

private:
    MainWindow _mainWindow;
    std::unique_ptr<focusrite::e2e::TestCentre> _testCentre = focusrite::e2e::TestCentre::create ();
};
