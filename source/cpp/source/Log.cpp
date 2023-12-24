#include <juce_core/juce_core.h>

#include <focusrite/e2e/Log.h>

namespace focusrite::e2e
{

void log(LogLevel logLevel, const juce::String& message)
{
    if (logLevel == LogLevel::silent)
        return;

    juce::Logger::writeToLog (message);
}

}