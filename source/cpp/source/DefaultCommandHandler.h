#pragma once

#include <focusrite/e2e/CommandHandler.h>

namespace focusrite::e2e
{
class DefaultCommandHandler : public CommandHandler
{
public:
    std::optional<Response> process (const Command & command) override;
};

}
