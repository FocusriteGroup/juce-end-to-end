#pragma once

#include <ampify/e2e/CommandHandler.h>

namespace ampify::e2e
{
class DefaultCommandHandler : public CommandHandler
{
public:
    std::optional<Response> process (const Command & command) override;
};

}
