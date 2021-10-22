#include <ampify/e2e/Response.h>
#include <catch2/catch_test_macros.hpp>

namespace ampify::e2e
{
TEST_CASE ("Converts to JSON", "[response]")
{
    const juce::Uuid uuid;
    const auto okResponse = Response::ok ()
                                .withUuid (uuid)
                                .withParameter ("string", "message")
                                .withParameter ("number", 12345);
    const auto failResponse =
        Response::fail ("Error message").withUuid (uuid).withParameter ("double", 0.123456789);

    const auto parsedOk = juce::JSON::parse (okResponse.toJson ());
    const auto parsedFail = juce::JSON::parse (failResponse.toJson ());

    SECTION ("Contains UUID")
    {
        REQUIRE (juce::Uuid (parsedOk.getProperty ("uuid", {})) == uuid);
    }

    SECTION ("Success")
    {
        REQUIRE (bool (parsedOk.getProperty ("success", {})));
        REQUIRE (! bool (parsedFail.getProperty ("success", {})));
    }

    SECTION ("Error message")
    {
        REQUIRE (parsedFail.getProperty ("error", {}) == "Error message");
    }

    SECTION ("String parameter")
    {
        const auto data = parsedOk.getProperty ("data", {});
        REQUIRE (data.getProperty ("string", {}) == "message");
    }

    SECTION ("Number parameter")
    {
        const auto data = parsedOk.getProperty ("data", {});
        REQUIRE (int (data.getProperty ("number", {})) == 12345);
    }

    SECTION ("Double parameter")
    {
        const auto data = parsedFail.getProperty ("data", {});
        REQUIRE (double (data.getProperty ("double", {})) == 0.123456789);
    }
}

}