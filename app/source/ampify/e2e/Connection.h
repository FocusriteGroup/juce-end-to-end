#pragma once

#include <array>
#include <juce_core/juce_core.h>
#include <mutex>

namespace ampify::e2e
{

class Connection
    : private juce::Thread
    , public std::enable_shared_from_this<Connection>
{
public:
    explicit Connection (int port);
    ~Connection () override;

    Connection (const Connection &) = delete;
    Connection (Connection &&) = delete;
    Connection & operator= (const Connection &) = delete;

    std::function<void (juce::MemoryBlock)> _onDataReceived;

    void start ();
    void send (const juce::MemoryBlock & data);
    bool isConnected () const;

private:
    int _port = 0;
    juce::StreamingSocket _socket;

    void run () override;

    void socketError ();
    void notifyData (const juce::MemoryBlock & data);
};

}

