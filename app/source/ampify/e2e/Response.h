#pragma once

#include <juce/JuceCore.h>

namespace ampify
{
namespace testing
{
class Response
{
public:
    Response (Uuid uuid, Result result);
    ~Response () = default;

    Response withParameter (String name, String value) const;

    String toString ();

    void addParameter (String name, String value);

private:
    Uuid _uuid;
    Result _result;
    StringPairArray _parameters;
};

}
}
