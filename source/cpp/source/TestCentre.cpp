#include "Connection.h"
#include "DefaultCommandHandler.h"

#include <ampify/e2e/Command.h>
#include <ampify/e2e/Event.h>
#include <ampify/e2e/TestCentre.h>
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

class E2ETestCentre final : public TestCentre
{
public:
    E2ETestCentre (LogLevel logLevel)
        : _logLevel (logLevel)
    {
        auto port = getPort ();
        if (! port)
            return;

        addCommandHandler (_defaultCommandHandler);

        _connection = Connection::create (*port);
        _connection->_onDataReceived = [this] (auto && block) { onDataReceived (block); };
        _connection->start ();
    }

    ~E2ETestCentre () override = default;

    void addCommandHandler (CommandHandler & handler) override
    {
        _commandHandlers.emplace_back (handler);
    }

    void removeCommandHandler (CommandHandler & handler) override
    {
        auto it = std::remove_if (_commandHandlers.begin (),
                                  _commandHandlers.end (),
                                  [&] (auto && other) { return &handler == &other.get (); });

        _commandHandlers.erase (it, _commandHandlers.end ());
    }

    void sendEvent (const Event & event) override
    {
        send (event.toJson ());
    }

private:
    void send (const juce::String & data)
    {
        if (_connection && _connection->isConnected ())
            _connection->send ({data.toRawUTF8 (), data.getNumBytesAsUTF8 ()});
    }

    void onDataReceived (const juce::MemoryBlock & data)
    {
        auto command = Command::fromJson (data.toString ());
        if (! command.isValid ())
            return;

        logCommand (command);

        bool responded = false;

        for (auto & commandHandler : _commandHandlers)
        {
            auto response = commandHandler.get ().process (command);
            if (! response)
                continue;

            logResponse (*response);
            send (response->withUuid (command.getUuid ()).toJson ());
            responded = true;

            if (command.getType () == "quit")
                juce::JUCEApplicationBase::quit ();
        }

        if (! responded)
            send (Response::fail ("Unhandled message").withUuid (command.getUuid ()).toJson ());
    }

    void logCommand (const Command & command)
    {
        if (_logLevel == LogLevel::silent)
            return;

        juce::Logger::writeToLog ("Received command: ");
        juce::Logger::writeToLog (command.describe ());
    }

    void logResponse (const Response & response)
    {
        if (_logLevel == LogLevel::silent)
            return;

        juce::Logger::writeToLog ("Sending response: ");
        juce::Logger::writeToLog (response.describe ());
    }

    const LogLevel _logLevel;

    DefaultCommandHandler _defaultCommandHandler;
    std::vector<std::reference_wrapper<CommandHandler>> _commandHandlers;
    std::shared_ptr<Connection> _connection;
};

std::unique_ptr<TestCentre> TestCentre::create (LogLevel logLevel)
{
    return std::make_unique<E2ETestCentre> (logLevel);
}

}