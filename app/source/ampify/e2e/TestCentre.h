#pragma once

#include "Command.h"
#include "CommandHandler.h"
#include "Connection.h"
#include "Event.h"
#include "Response.h"

#include <memory>
#include <optional>

namespace ampify::e2e
{
class TestCentre
{
public:
    explicit TestCentre ();
    ~TestCentre ();

    void addCommandHandler (std::shared_ptr<CommandHandler> handler);
    void sendEvent (const Event & event);

private:
    void send (const juce::String & data);

    void onDataReceived (const juce::MemoryBlock & data);

    std::vector<std::shared_ptr<CommandHandler>> _commandHandlers;
    std::shared_ptr<Connection> _connection;
};

}
