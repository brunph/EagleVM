#pragma once
#include "eaglevm-core/virtual_machine/handlers/handler/base_handler_entry.h"
#include "eaglevm-core/virtual_machine/handlers/handler/vm_handler_entry.h"

enum class encode_status
{
    success,
    unsupported
};

struct encode_ctx
{
    int32_t* stack_disp;
    uint64_t orig_rva;
    uint8_t index;
};

class inst_handler_entry : public base_handler_entry
{
public:
    inst_handler_entry(vm_inst_regs* manager, vm_inst_handlers* handler_generator)
        : base_handler_entry(manager, handler_generator) { }

    virtual std::pair<bool, function_container> translate_to_virtual(const zydis_decode& decoded_instruction, uint64_t original_rva);
    code_label* get_handler_va(reg_size width, uint8_t operands) const;

    virtual bool virtualize_as_address(const zydis_decode& inst, int index);

protected:
    virtual encode_status encode_operand(
        function_container& container, const zydis_decode& instruction, zydis_dreg op_reg, encode_ctx& context);
    virtual encode_status encode_operand(
        function_container& container, const zydis_decode& instruction, zydis_dmem op_mem, encode_ctx& context);
    virtual encode_status encode_operand(
        function_container& container, const zydis_decode& instruction, zydis_dptr op_ptr, encode_ctx& context);
    virtual encode_status encode_operand(
        function_container& container, const zydis_decode& instruction, zydis_dimm op_imm, encode_ctx& context);

    virtual void finalize_translate_to_virtual(const zydis_decode& decoded_instruction, function_container& container);
};