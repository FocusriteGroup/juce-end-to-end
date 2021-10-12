#include "TestCentre.h"

#include "Command.h"
#include "DefaultCommandHandler.h"

#include <juce_events/juce_events.h>

namespace ampify::e2e
{
std::optional<int> getPort ()
{
    std::optional<int> port;

    for (const auto & param : juce::JUCEApplicationBase::getCommandLineParameterArray ())
        if (param.contains ("--test-fixture-port="))
            port = param.trimCharactersAtStart ("--test-fixture-port=").getIntValue ();

    return port;
}

TestCentre::TestCentre ()
{
    auto port = getPort ();
    if (! port)
        return;

    addCommandHandler (std::make_shared<DefaultCommandHandler> ());

    _connection = std::make_shared<Connection> (*port);

    _connection->_onDataReceived = [this] (auto && block) { onDataReceived (block); };

    _connection->start ();
}

TestCentre::~TestCentre ()
{
    _connection->_onDataReceived = nullptr;
    _connection.reset ();
}

void TestCentre::addCommandHandler (std::shared_ptr<CommandHandler> handler)
{
    _commandHandlers.push_back (std::move (handler));
}

void TestCentre::sendEvent (const Event & event)
{
    send (event.toString ());
}

void TestCentre::send (const juce::String & data)
{
    if (_connection->isConnected ())
        _connection->send ({data.toRawUTF8 (), data.getNumBytesAsUTF8 ()});
}

void TestCentre::onDataReceived (const juce::MemoryBlock & data)
{
    auto command = Command::fromString (data.toString ());
    if (command.isValid ())
    {
        bool responded = false;

        for (auto & commandHandler : _commandHandlers)
        {
            auto response = commandHandler->process (command);
            if (! response)
                continue;

            send (response->toString ());
            responded = true;

            if (command.getType () == Command::Type::quit)
                juce::JUCEApplicationBase::quit ();

            break;
        }

        if (! responded)
        {
            Response response (command.getUuid (), juce::Result::fail ("Unhandled message"));
            send (response.toString ());
        }
    }
}

}
