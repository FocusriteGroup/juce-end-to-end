#include <focusrite/e2e/Event.h>

namespace focusrite::e2e
{
Event::Event (juce::String name)
    : _name (std::move (name))
{
}

Event Event::withParameter (const juce::String & name, const juce::var & value) const
{
    Event other (*this);
    other.addParameter (name, value);
    return other;
}

juce::String Event::toJson () const
{
    auto root = std::make_unique<juce::DynamicObject> ();

    root->setProperty ("type", "event");
    root->setProperty ("name", _name);

    if (! _parameters.empty ())
    {
        auto data = std::make_unique<juce::DynamicObject> ();

        for (auto & [name, value] : _parameters)
            data->setProperty (name, value);

        root->setProperty ("data", data.release ());
    }

    return juce::JSON::toString (root.release ());
}

void Event::addParameter (const juce::String & name, const juce::var & value)
{
    _parameters [name] = value;
}

}
