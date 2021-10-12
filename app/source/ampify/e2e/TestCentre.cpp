#include "TestCentre.h"

namespace ampify
{
namespace testing
{
TestCentre::TestCentre (int port)
    : _connection (std::make_shared<Connection> (port))
{
    _connection->_onDataReceived = [this] (MemoryBlock block)
    {
        ASSERT_IS_ON_JUCE_MESSAGE_THREAD;

        auto command = Command::fromString (block.toString ());
        if (command.isValid () && _commandHandler)
        {
            auto response = _commandHandler (command);
            send (response.toString ());

            if (command.getType () == Command::Type::quit)
                JUCEApplication::getInstance ()->quit ();
        }
    };

    _connection->start ();
}

TestCentre::~TestCentre ()
{
    _connection->_onDataReceived = nullptr;
    _connection.reset ();
}

void TestCentre::setCommandHandler (CommandHandler handler)
{
    _commandHandler = handler;
}

void TestCentre::sendEvent (Event event)
{
    send (event.toString ());
}

void TestCentre::send (const String & data)
{
    if (_connection->isConnected ())
        _connection->send (MemoryBlock (data.toRawUTF8 (), data.getNumBytesAsUTF8 ()));
}

}
}
