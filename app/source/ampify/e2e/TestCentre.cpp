#include "TestCentre.h"

#include "Command.h"
#include "DefaultCommandHandler.h"

#include <juce_events/juce_events.h>

namespace ampify::e2e
{
std::optional<int> getPort ()
{
    for (const auto & param : juce::JUCEApplicationBase::getCommandLineParameterArray ())
    {
        if (param.contains ("--e2e-test-port="))
        {
            int port = param.trimCharactersAtStart ("--e2e-test-port=").getIntValue ();
            if (std::numeric_limits<uint16_t>::min () <= port &&
                port <= std::numeric_limits<uint16_t>::max ())
                return port;
        }
    }

    return std::nullopt;
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
    if (! command.isValid ())
        return;

    bool responded = false;

    for (auto & commandHandler : _commandHandlers)
    {
        auto response = commandHandler->process (command);
        if (! response)
            continue;

        send (response->withUuid (command.getUuid ()).toString ());
        responded = true;

        if (command.getType () == "quit")
            juce::JUCEApplicationBase::quit ();

        break;
    }

    if (! responded)
        send (Response::fail ("Unhandled message").withUuid (command.getUuid ()).toString ());
}

}
