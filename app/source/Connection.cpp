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
std::shared_ptr<Connection> Connection::create (int port)
{
    return std::shared_ptr<Connection> (new Connection (port));
}

Connection::Connection (int port)
    : Thread ("Test fixture connection")
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

    const auto connected = _socket.connect ("localhost", _port);

    if (! connected)
        return;

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

    Header header {juce::ByteOrder::swapIfBigEndian (Header::magicNumber),
                   juce::ByteOrder::swapIfBigEndian (uint32_t (data.getSize ()))};

    if (! writeBytesToSocket (_socket, &header, sizeof (header)))
    {
        closeSocket ();
        return;
    }

    if (! writeBytesToSocket (_socket, data.getData (), int (data.getSize ())))
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
