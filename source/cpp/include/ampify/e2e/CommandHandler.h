#pragma once
#include <ampify/e2e/Response.h>
#include <optional>

namespace ampify::e2e
{
class Command;

class CommandHandler
{
public:
    virtual ~CommandHandler () = default;

    virtual std::optional<Response> process (const Command & command) = 0;
};

}