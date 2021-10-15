#pragma once

#include "CommandHandler.h"

namespace ampify::e2e
{
class DefaultCommandHandler : public CommandHandler
{
public:
    std::optional<Response> process (const Command & command) override;
};

}
