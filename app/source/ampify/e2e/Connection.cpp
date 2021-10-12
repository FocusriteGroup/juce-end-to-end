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

bool writeBytesToSocket (juce::StreamingSocket & socket, const void * data, int numBytes)
{
    while (numBytes > 0)
    {
        const auto numBytesWritten = socket.write (data, numBytes);

        if (numBytesWritten < 0)
            return false;

        numBytes -= numBytesWritten;
    }

    return true;
}

}

namespace ampify::e2e
{
Connection::Connection (int port)
    : Thread ("Test fixture connection")
    , _port (port)
{
}

Connection::~Connection ()
{
    if (_socket.isConnected ())
        _socket.close ();

    constexpr int timeoutMs = 1000;
    stopThread (timeoutMs);
}

void Connection::start ()
{
    startThread ();
}

void Connection::run ()
{
#if JUCE_MAC
    // Stop socket errors from triggering a SIGPIPE signal
    auto socketFd = _socket.getRawSocketHandle ();
    const int set = 1;
    setsockopt (socketFd, SOL_SOCKET, SO_NOSIGPIPE, (void *) &set, sizeof (int));
#endif

    bool connected = _socket.connect ("localhost", _port);

    if (! connected)
        return;

    juce::Logger::writeToLog ("Connected to test fixture");

    try
    {
        while (! threadShouldExit ())
        {
            Header header;

            auto headerBytesRead = _socket.read (&header, sizeof (header), true);

            if (headerBytesRead != sizeof (header))
            {
                socketError ();
                break;
            }

            header.magic = juce::ByteOrder::swapIfBigEndian (header.magic);
            header.size = juce::ByteOrder::swapIfBigEndian (header.size);

            if (header.magic != Header::magicNumber)
            {
                socketError ();
                break;
            }

            juce::MemoryBlock block (header.size);
            auto bytesRead = _socket.read (block.getData (), int (header.size), true);
            if (bytesRead != int (header.size))
            {
                socketError ();
                break;
            }

            notifyData (block);
        }
    }
    catch (...)
    {
        juce::Logger::outputDebugString ("Connection threw exception...");
    }
}

void Connection::send (const juce::MemoryBlock & data)
{
    jassert (isConnected ());

    Header header;
    header.magic = juce::ByteOrder::swapIfBigEndian (Header::magicNumber);
    header.size = juce::ByteOrder::swapIfBigEndian (uint32_t (data.getSize ()));

    if (! writeBytesToSocket (_socket, &header, sizeof (header)))
    {
        socketError ();
        return;
    }

    if (! writeBytesToSocket (_socket, data.getData (), int (data.getSize ())))
        socketError ();
}

bool Connection::isConnected () const
{
    return _socket.isConnected ();
}

void Connection::socketError ()
{
    if (_socket.isConnected ())
        _socket.close ();
}

void Connection::notifyData (const juce::MemoryBlock & data)
{
    std::weak_ptr<Connection> self = shared_from_this ();
    juce::MessageManager::callAsync (
        [self, data]
        {
            if (auto connection = self.lock ())
            {
                if (connection->_onDataReceived)
                    connection->_onDataReceived (data);
            }
        });
}

}
