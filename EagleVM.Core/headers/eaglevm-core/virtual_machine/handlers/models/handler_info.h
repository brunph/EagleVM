#pragma once
#include "eaglevm-core/util/zydis_helper.h"

class code_label;
struct handler_info
{
    reg_size instruction_width = bit64;
    uint8_t operand_count = 2;

    code_label* target_label = nullptr;
};