#pragma once

#include <ampify/e2e/CommandHandler.h>
#include <memory>
#include <optional>

namespace ampify::e2e
{
class Event;
class TestCentre
{
public:
    enum class LogLevel
    {
        silent,
        verbose,
    };

    static std::unique_ptr<TestCentre> create (LogLevel logLevel = LogLevel::silent);


    virtual ~TestCentre () = default;

    virtual void addCommandHandler (CommandHandler & handler) = 0;
    virtual void removeCommandHandler (CommandHandler & handler) = 0;

    virtual void sendEvent (const Event & event) = 0;
};

}
