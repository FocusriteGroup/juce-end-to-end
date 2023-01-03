#include <focusrite/e2e/Response.h>

namespace focusrite::e2e
{
Response Response::ok ()
{
    return Response (juce::Result::ok ());
}

Response Response::fail (const juce::String & message)
{
    return Response (juce::Result::fail (message));
}

Response::Response (juce::Result result)
    : _result (std::move (result))
{
}

Response Response::withParameter (const juce::String & name, const juce::var & value) const
{
    Response other (*this);
    other.addParameter (name, value);
    return other;
}

Response Response::withUuid (const juce::Uuid & uuid) const
{
    Response other (*this);
    other._uuid = uuid;
    return other;
}

juce::String Response::toJson () const
{
    auto element = std::make_unique<juce::DynamicObject> ();
    element->setProperty ("type", "response");
    element->setProperty ("uuid", _uuid.toDashedString ());
    element->setProperty ("success", _result.wasOk ());

    if (! _result)
        element->setProperty ("error", _result.getErrorMessage ());

    if (! _parameters.empty ())
    {
        auto data = std::make_unique<juce::DynamicObject> ();

        for (const auto & [key, value] : _parameters)
            data->setProperty (key, value);

        element->setProperty ("data", data.release ());
    }

    return juce::JSON::toString (element.release ());
}

juce::String Response::describe () const
{
    juce::String description;

    description << "Success: " << juce::String (_result.wasOk () ? "true" : "false")
                << juce::newLine;

    if (! _result)
        description << "Error: " << _result.getErrorMessage ();

    for (auto && [key, value] : _parameters)
        description << key << ": " << value.toString () << juce::newLine;

    return description;
}

void Response::addParameter (const juce::String & name, const juce::var & value)
{
    _parameters [name] = value;
}

}
