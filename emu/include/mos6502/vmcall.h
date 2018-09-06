#pragma once

#include <base.h>
#include <mos6502/mos6502.h>

// VMCALL numbers
#define VMCALL_ARGS 0
#define VMCALL_EXIT 1
#define VMCALL_OPEN 2
#define VMCALL_CLOSE 3
#define VMCALL_READ 4
#define VMCALL_WRITE 5
#define VMCALL_BREAK 6

// Handles a VMCALL opcode
mos6502_step_result_t handle_vmcall (decode_info_t * nonnull info);
