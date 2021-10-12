#pragma once

#include "Command.h"
#include "Connection.h"
#include "Event.h"
#include "Response.h"

namespace ampify
{
namespace testing
{
class TestCentre
{
public:
    TestCentre (int port);
    ~TestCentre ();

    using CommandHandler = std::function<Response (Command command)>;

    void setCommandHandler (CommandHandler handler);
    void sendEvent (Event event);

private:
    TestCentre (const TestCentre &) = delete;
    TestCentre (TestCentre &&) = delete;
    TestCentre & operator= (const TestCentre & other) = delete;

    void send (const String & data);

    CommandHandler _commandHandler = nullptr;
    std::shared_ptr<Connection> _connection;
};

}
}
