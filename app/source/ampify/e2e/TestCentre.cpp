#include "TestCentre.h"

#include <juce_events/juce_events.h>

namespace ampify::e2e_testing
{
TestCentre::TestCentre (int port)
    : _connection (std::make_shared<Connection> (port))
{
    _connection->_onDataReceived = [this] (auto && block)
    {
        auto command = Command::fromString (block.toString ());
        if (command.isValid () && _commandHandler)
        {
            auto response = _commandHandler (command);
            send (response.toString ());

            if (command.getType () == Command::Type::quit)
                juce::JUCEApplicationBase::quit ();
        }
    };

    _connection->start ();
}

TestCentre::~TestCentre ()
{
    _connection->_onDataReceived = nullptr;
    _connection.reset ();
}

void TestCentre::addCommandHandler (CommandHandler handler)
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
        _connection->send (juce::MemoryBlock (data.toRawUTF8 (), data.getNumBytesAsUTF8 ()));
}

}
