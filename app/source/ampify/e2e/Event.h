#pragma once

#include <juce/JuceCore.h>
#include <map>

namespace ampify
{
namespace testing
{
class Event
{
public:
    Event (String name);
    Event (const Event & other) = default;
    ~Event () = default;

    Event withParameter (const String & name, const String & value) const;

    String toString () const;

    void addParameter (const String & name, const var & value);

private:
    String _name;
    std::map<String, var> _parameters;
};

}
}
