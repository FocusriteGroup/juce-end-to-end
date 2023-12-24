#pragma once

#include <juce_core/juce_core.h>

#include <focusrite/e2e/Log.h>

namespace focusrite::e2e
{
class Connection
    : private juce::Thread
    , public std::enable_shared_from_this<Connection>
{
public:
    static std::shared_ptr<Connection> create (LogLevel logLevel, int port);

    ~Connection () override;

    Connection (const Connection &) = delete;
    Connection (Connection &&) = delete;
    Connection & operator= (const Connection &) = delete;

    std::function<void (juce::MemoryBlock)> _onDataReceived;

    void start ();
    void send (const juce::MemoryBlock & data);
    [[nodiscard]] bool isConnected () const;

private:
    explicit Connection (LogLevel logLevel, int port);

    void run () override;

    void closeSocket ();
    void preventSigPipeExceptions ();
    void notifyData (const juce::MemoryBlock & data);

    LogLevel _logLevel { LogLevel::silent };
    int _port = 0;
    juce::StreamingSocket _socket;
};

}
