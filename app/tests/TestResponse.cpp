#include <ampify/e2e/Response.h>

namespace ampify::e2e
{
struct Fixture
{
public:
    juce::Uuid uuid;
    const Response okResponse = Response::ok ()
                                    .withUuid (uuid)
                                    .withParameter ("string", "message")
                                    .withParameter ("number", 12345);
    const Response failResponse =
        Response::fail ("Error message").withUuid (uuid).withParameter ("double", 0.123456789);

    const juce::var parsedOk = juce::JSON::parse (okResponse.toJson ());
    const juce::var parsedFail = juce::JSON::parse (failResponse.toJson ());
};

class ResponseTests final : public juce::UnitTest
{
public:
    ResponseTests () noexcept
        : juce::UnitTest ("Response")
    {
    }

    void initialise () override
    {
        _fixture = std::make_unique<Fixture> ();
    }

    void shutdown () override
    {
        _fixture.reset ();
    }

    void runTest () override
    {
        struct Test
        {
            juce::String name;
            std::function<void ()> entry;
        };

        auto tests = {
            Test {"UUID", [this] { containsUuid (); }},
            Test {"Success", [this] { containsSuccess (); }},
            Test {"Error message", [this] { containsErrorMessage (); }},
            Test {"String parameter", [this] { stringParameter (); }},
            Test {"Number parameter", [this] { numberParameter (); }},
            Test {"Double parameter", [this] { doubleParameter (); }},
        };

        for (auto && test : tests)
        {
            beginTest (test.name);

            test.entry ();
        }
    }

    void containsUuid ()
    {
        expect (juce::Uuid (_fixture->parsedOk.getProperty ("uuid", {})) == _fixture->uuid);
    }

    void containsSuccess ()
    {
        expect (bool (_fixture->parsedOk.getProperty ("success", {})));
        expect (! bool (_fixture->parsedFail.getProperty ("success", {})));
    }

    void containsErrorMessage ()
    {
        expectEquals (_fixture->parsedFail.getProperty ("error", {}).toString (),
                      juce::String ("Error message"));
    }

    void stringParameter ()
    {
        const auto data = _fixture->parsedOk.getProperty ("data", {});
        expectEquals (data.getProperty ("string", {}).toString (), juce::String ("message"));
    }

    void numberParameter ()
    {
        const auto data = _fixture->parsedOk.getProperty ("data", {});
        expectEquals (int (data.getProperty ("number", {})), 12345);
    }

    void doubleParameter ()
    {
        const auto data = _fixture->parsedFail.getProperty ("data", {});
        expectWithinAbsoluteError (double (data.getProperty ("double", {})), 0.123456789, 1e-9);
    }

private:
    std::unique_ptr<Fixture> _fixture;
};

[[maybe_unused]] static ResponseTests responseTests;

}