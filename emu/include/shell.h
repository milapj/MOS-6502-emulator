#pragma once

#include <mos6502/mos6502.h>

#define PROMPT_STR GREEN(BOLD("(hawknest-shell)$> "))

void run_shell (mos6502_t * nonnull cpu, bool interactive);
