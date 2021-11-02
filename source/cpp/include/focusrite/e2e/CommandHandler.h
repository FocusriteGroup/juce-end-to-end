#pragma once
#include <focusrite/e2e/Response.h>
#include <optional>

namespace focusrite::e2e
{
class Command;

class CommandHandler
{
public:
    virtual ~CommandHandler () = default;

    virtual std::optional<Response> process (const Command & command) = 0;
};

}