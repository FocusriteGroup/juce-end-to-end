#pragma once

#include <memory>
#include <optional>

#include <focusrite/e2e/Log.h>
#include <focusrite/e2e/CommandHandler.h>

namespace focusrite::e2e
{
class Event;
class TestCentre
{
public:
    using LogLevel = focusrite::e2e::LogLevel;
    
    static std::unique_ptr<TestCentre> create (LogLevel logLevel = LogLevel::silent, uint16_t port = 0);

    virtual ~TestCentre () = default;

    virtual void addCommandHandler (CommandHandler & handler) = 0;
    virtual void removeCommandHandler (CommandHandler & handler) = 0;

    virtual void sendEvent (const Event & event) = 0;
};

}
