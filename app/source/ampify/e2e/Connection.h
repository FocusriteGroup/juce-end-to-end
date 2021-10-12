#pragma once

#include <array>
#include <juce/JuceCore.h>
#include <mutex>

namespace ampify
{
namespace testing
{
class Connection
    : private Thread
    , public std::enable_shared_from_this<Connection>
{
public:
    Connection (int port);
    ~Connection ();

    Connection (const Connection &) = delete;
    Connection (Connection &&) = delete;
    Connection & operator= (const Connection &) = delete;

    std::function<void (MemoryBlock)> _onDataReceived;

    void start ();
    void send (MemoryBlock data);
    bool isConnected () const;

private:
    int _port = 0;
    StreamingSocket _socket;

    void run () override;

    void socketError ();
    void notifyData (MemoryBlock data);
};

}
}
