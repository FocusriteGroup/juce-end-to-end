#pragma once

#include "CommandHandler.h"
#include "Connection.h"

#include <memory>
#include <optional>

namespace ampify::e2e
{
class Event;
class TestCentre
{
public:
    TestCentre ();
    ~TestCentre () = default;

    void addCommandHandler (std::shared_ptr<CommandHandler> handler);
    void sendEvent (const Event & event);

private:
    void send (const juce::String & data);

    void onDataReceived (const juce::MemoryBlock & data);

    std::vector<std::shared_ptr<CommandHandler>> _commandHandlers;
    std::shared_ptr<Connection> _connection;
};

}
