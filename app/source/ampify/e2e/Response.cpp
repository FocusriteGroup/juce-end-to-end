#include "Response.h"

#include <ampify/utils/json/JsonDocument.h>
#include <ampify/utils/json/JsonElement.h>

namespace ampify
{
namespace testing
{
Response::Response (Uuid uuid, Result result)
    : _uuid (uuid)
    , _result (result)
{
}

Response Response::withParameter (String name, String value) const
{
    Response other (*this);
    other.addParameter (name, value);
    return other;
}

String Response::toString ()
{
    JsonElement rootElement;

    rootElement.setStringAttribute ("type", "response");
    rootElement.setStringAttribute ("uuid", _uuid.toDashedString ());
    rootElement.setBoolAttribute ("success", _result.wasOk ());

    if (! _result)
        rootElement.setStringAttribute ("error", _result.getErrorMessage ());

    auto keys = _parameters.getAllKeys ();

    if (keys.size () > 0)
    {
        auto * dataElement = new DynamicObject;

        for (auto & key : keys)
            dataElement->setProperty (key, _parameters [key]);

        rootElement.setChildElement ("data", JsonElement (var (dataElement)));
    }

    return JsonDocument (rootElement).toString ();
}

void Response::addParameter (String name, String value)
{
    _parameters.set (name, value);
}

}
}
