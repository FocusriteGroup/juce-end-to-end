#include <ampify/e2e/Command.h>
#include <juce_core/juce_core.h>

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
class CommandTests final : public juce::UnitTest
{
public:
    CommandTests () noexcept
        : juce::UnitTest ("Command")
    {
    }

    void runTest () override
    {
        struct Test
        {
            juce::String name;
            std::function<void ()> entry;
        };

        auto tests = {
            Test {"Converts type from JSON", [this] { convertsTypeFromJson (); }},
            Test {"Converts UUID from JSON", [this] { convertsUuidFromJson (); }},
            Test {"Converts String argument from JSON",
                  [this] { convertsStringArgumentFromJson (); }},
            Test {"Converts int argument from JSON", [this] { convertsIntArgumentFromJson (); }},
            Test {"Converts bool argument from JSON", [this] { convertsBoolArgumentFromJson (); }},
            Test {"Converts object argument from JSON",
                  [this] { convertsObjectArgumentFromJson (); }},
        };

        for (auto && test : tests)
        {
            beginTest (test.name);
            test.entry ();
        }
    }

    void convertsTypeFromJson ()
    {
        expectEquals (command.getType (), juce::String ("command-type"));
    }

    void convertsUuidFromJson ()
    {
        expect (command.getUuid () == juce::Uuid ("beb16073-dbcd-49aa-b7d1-9466582a1e0e"));
    }

    void convertsStringArgumentFromJson ()
    {
        expectEquals (command.getArgument ("string-type"), juce::String ("string argument"));
    }

    void convertsIntArgumentFromJson ()
    {
        expectEquals (command.getArgumentAs<int> ("int-type"), 45675);
    }

    void convertsBoolArgumentFromJson ()
    {
        expect (command.getArgumentAs<bool> ("bool-type"));
    }

    void convertsObjectArgumentFromJson ()
    {
        const auto var = command.getArgumentAsVar ("object-type");
        expectEquals (var.getProperty ("value", {}).toString (), juce::String ("object"));
    }

private:
    const Command command = Command::fromJson (exampleJson);
};

[[maybe_unused]] static CommandTests commandTests;

}