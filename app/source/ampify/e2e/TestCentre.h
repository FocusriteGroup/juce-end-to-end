#pragma once

#include "Command.h"
#include "Connection.h"
#include "Event.h"
#include "Response.h"

namespace ampify::e2e_testing
{
class TestCentre
{
public:
    explicit TestCentre (int port);
    ~TestCentre ();

    using CommandHandler = std::function<Response (Command command)>;

    void addCommandHandler (CommandHandler handler);
    void sendEvent (const Event & event);

private:
    void send (const juce::String & data);

    CommandHandler _commandHandler = nullptr;
    std::vector<CommandHandler> _commandHandlers;
    std::shared_ptr<Connection> _connection;
};

}
