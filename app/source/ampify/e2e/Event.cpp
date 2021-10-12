#include "Event.h"

#include <ampify/utils/json/JsonDocument.h>
#include <ampify/utils/json/JsonElement.h>

namespace ampify
{
namespace testing
{
Event::Event (String name)
    : _name (name)
{
}

Event Event::withParameter (const String & name, const String & value) const
{
    Event other (*this);
    other.addParameter (name, value);
    return other;
}

String Event::toString () const
{
    JsonElement rootElement;

    rootElement.setStringAttribute ("type", "event");
    rootElement.setStringAttribute ("name", _name);

    if (! _parameters.empty ())
    {
        auto * dataElement = new DynamicObject;

        for (auto & parameter : _parameters)
            dataElement->setProperty (parameter.first, parameter.second);

        rootElement.setChildElement ("data", JsonElement (var (dataElement)));
    }

    return JsonDocument (rootElement).toString ();
}

void Event::addParameter (const String & name, const var & value)
{
    _parameters [name] = value;
}

}
}
