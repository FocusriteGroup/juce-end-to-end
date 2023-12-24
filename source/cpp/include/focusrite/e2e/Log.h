#pragma once

namespace focusrite::e2e
{

enum class LogLevel
{
    silent,
    verbose,
};

void log(LogLevel logLevel, const juce::String& message);

}
