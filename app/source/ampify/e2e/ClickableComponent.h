#pragma once

namespace ampify::e2e
{
class ClickableComponent
{
public:
    virtual ~ClickableComponent () = default;

    virtual void performClick ()
    {
    }

    virtual void performDoubleClick ()
    {
    }

    virtual void performRightClick ()
    {
    }
};

}
