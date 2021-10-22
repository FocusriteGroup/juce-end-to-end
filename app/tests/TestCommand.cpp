#include <ampify/e2e/Command.h>
#include <catch2/catch_test_macros.hpp>

const auto exampleJson = R"identifier(
{
    "type": "command-type",
    "uuid": "beb16073-dbcd-49aa-b7d1-9466582a1e0e",
    "args": {
        "string-type": "string argument",
        "int-type": 45675,
        "bool-type": true,
        "object-type": {
            "value": "object",
        }
    }
}
)identifier";

namespace ampify::e2e
{
TEST_CASE ("Converts from JSON", "[command]")
{
    auto command = Command::fromJson (exampleJson);
    REQUIRE (command.isValid ());

    SECTION ("Type argument")
    {
        REQUIRE (command.getType () == "command-type");
    }

    SECTION ("UUID")
    {
        REQUIRE (command.getUuid () == juce::Uuid ("beb16073-dbcd-49aa-b7d1-9466582a1e0e"));
    }

    SECTION ("String argument")
    {
        REQUIRE (command.getArgument ("string-type") == "string argument");
    }

    SECTION ("Int argument")
    {
        REQUIRE (command.getArgumentAs<int> ("int-type") == 45675);
    }

    SECTION ("Bool argument")
    {
        REQUIRE (command.getArgumentAs<bool> ("bool-type"));
    }

    SECTION ("Object argument")
    {
        const auto var = command.getArgumentAsVar ("object-type");
        REQUIRE (var.getProperty ("value", {}).toString () == "object");
    }
}
}