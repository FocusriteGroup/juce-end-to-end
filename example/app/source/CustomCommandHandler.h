#pragma once

#include <cstdlib>
#include <focusrite/e2e/CommandHandler.h>

class CustomCommandHandler : public focusrite::e2e::CommandHandler
{
public:
    std::optional<focusrite::e2e::Response>
    process (const focusrite::e2e::Command & command) override
    {
        if (command.getType () == "abort")
            std::abort ();

        return {};
    }
};