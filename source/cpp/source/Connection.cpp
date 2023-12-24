#include "Connection.h"

#include <juce_events/juce_events.h>

#if JUCE_MAC
    #include <sys/socket.h>
#endif

namespace
{
#pragma pack(push, 1)
struct Header
{
    static constexpr uint32_t magicNumber = 0x30061990;

    uint32_t magic = 0;
    uint32_t size = 0;
};
#pragma pack(pop)

static_assert (sizeof (Header) == 2 * sizeof (uint32_t), "Expecting header to be 8 bytes");

bool writeBytesToSocket (juce::StreamingSocket & socket, const juce::MemoryBlock & data)
{
    int offset = 0;

    while (static_cast<size_t> (offset) < data.getSize ())
    {
        const auto numBytesWritten =
            socket.write (&data [offset], static_cast<int> (data.getSize ()) - offset);

        if (numBytesWritten < 0)
            return false;

        offset += numBytesWritten;
    }

    return true;
}

}

namespace focusrite::e2e
{
std::shared_ptr<Connection> Connection::create (LogLevel logLevel, int port)
{
    return std::shared_ptr<Connection> (new Connection (logLevel, port));
}

Connection::Connection (LogLevel logLevel, int port)
    : Thread ("Test fixture connection")
    , _logLevel (logLevel)
    , _port (port)
{
}

Connection::~Connection ()
{
    closeSocket ();

    constexpr int timeoutMs = 1000;
    stopThread (timeoutMs);
}

void Connection::start ()
{
    startThread ();
}

void Connection::run ()
{
    preventSigPipeExceptions ();

    log (_logLevel, "Connecting to port: " + juce::String(_port));        

    bool connected = false; 

    while (! threadShouldExit () && ! connected)
    {
        connected = _socket.connect ("localhost", _port);
        if (! connected)
        {
            log (_logLevel, "Failed to connect, waiting and retrying");
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1000ms);
        }
    }

    if (connected)
        log (_logLevel, "Connected");

    try
    {
        while (! threadShouldExit ())
        {
            Header header;

            auto headerBytesRead = _socket.read (&header, sizeof (header), true);

            if (headerBytesRead != sizeof (header))
            {
                closeSocket ();
                break;
            }

            header.magic = juce::ByteOrder::swapIfBigEndian (header.magic);
            header.size = juce::ByteOrder::swapIfBigEndian (header.size);

            if (header.magic != Header::magicNumber)
            {
                closeSocket ();
                break;
            }

            juce::MemoryBlock block (header.size);
            auto bytesRead = _socket.read (block.getData (), int (header.size), true);
            if (bytesRead != int (header.size))
            {
                closeSocket ();
                break;
            }

            notifyData (block);
        }
    }
    catch (...)
    {
    }
}

void Connection::send (const juce::MemoryBlock & data)
{
    jassert (isConnected ());

    if (const Header header {juce::ByteOrder::swapIfBigEndian (Header::magicNumber),
                             juce::ByteOrder::swapIfBigEndian (uint32_t (data.getSize ()))};
        ! writeBytesToSocket (_socket, {&header, sizeof (header)}))
    {
        closeSocket ();
        return;
    }

    if (! writeBytesToSocket (_socket, data))
        closeSocket ();
}

bool Connection::isConnected () const
{
    return _socket.isConnected ();
}

void Connection::closeSocket ()
{
    if (_socket.isConnected ())
        _socket.close ();
}

void Connection::notifyData (const juce::MemoryBlock & data)
{
    juce::MessageManager::callAsync (
        [weakSelf = weak_from_this (), data]
        {
            if (auto connection = weakSelf.lock ())
                if (connection->_onDataReceived)
                    connection->_onDataReceived (data);
        });
}

void Connection::preventSigPipeExceptions ()
{
#if JUCE_MAC
    auto socketFd = _socket.getRawSocketHandle ();
    const int set = 1;
    setsockopt (socketFd, SOL_SOCKET, SO_NOSIGPIPE, (void *) &set, sizeof (int));
#endif
}

}
