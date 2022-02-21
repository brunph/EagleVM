#include "virtual_machine/vm_generator.h"

#define VIP         rm_.reg_map[I_VIP]
#define VSP         rm_.reg_map[I_VSP]
#define VREGS       rm_.reg_map[I_VREGS]
#define VTEMP       rm_.reg_map[I_VTEMP]
#define PUSHORDER   rm_.reg_stack_order_

vm_generator::vm_generator()
{
    zydis_helper::setup_decoder();
    hg_ = vm_handle_generator(&rm_);
}

void vm_generator::init_reg_order()
{
    rm_.init_reg_order();
}

void vm_generator::init_ran_consts()
{
    hg_.setup_enc_constants();
}

void vm_generator::generate_vm_handlers(uint32_t va_of_section)
{
    hg_.setup_vm_mapping();

    uint32_t current_section_offset = 0;
    std::printf("%3s %-17s %-10s\n", "", "handler", "instructions");
    for (auto&[k, v] : hg_.vm_handlers)
    {
        for (int i = 0; i < 4; i++)
        {
            reg_size supported_size = v.supported_handler_va[i];
            if (supported_size == reg_size::unsupported)
                break;

            auto instructions = v.creation_binder(supported_size);
            auto encoded = zydis_helper::encode_queue(instructions);

            section_data_ += encoded;
            v.handler_va[i] = current_section_offset;
            current_section_offset += encoded.size();
        }
    }
}

std::vector<zydis_encoder_request> vm_generator::translate_to_virtual(const zydis_decode& decoded_instruction)
{
    //virtualizer does not support more than 2 operands OR all mnemonics
    if (decoded_instruction.instruction.operand_count > 2 ||
        !hg_.vm_handlers.contains(decoded_instruction.instruction.mnemonic))
    {
        INSTRUCTION_NOT_SUPPORTED:
        return {};
    }

    std::vector<zydis_encoder_request> virtualized_instruction;
    for (int i = 0; i < decoded_instruction.instruction.operand_count; i++)
    {
        encode_data encode;
        switch (const zydis_decoded_operand operand = decoded_instruction.operands[i]; operand.type)
        {
            case ZYDIS_OPERAND_TYPE_UNUSED:
                break;
            case ZYDIS_OPERAND_TYPE_REGISTER:
                encode = encode_operand(decoded_instruction, operand.reg);
                break;
            case ZYDIS_OPERAND_TYPE_MEMORY:
                encode = encode_operand(decoded_instruction, operand.mem);
                break;
            case ZYDIS_OPERAND_TYPE_POINTER:
                encode = encode_operand(decoded_instruction, operand.ptr);
                break;
            case ZYDIS_OPERAND_TYPE_IMMEDIATE:
                encode = encode_operand(decoded_instruction, operand.imm);
                break;
        }

        if (encode.second != encode_status::success)
            goto INSTRUCTION_NOT_SUPPORTED;

        virtualized_instruction += encode.first;
    }

    return virtualized_instruction;
}

std::vector<uint8_t> vm_generator::create_padding(const size_t bytes)
{
    std::vector<uint8_t> padding(bytes);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, UINT8_MAX);

    for (int i = 0; i < bytes; i++)
        padding.push_back(dist(rng));

    return padding;
}

encode_data vm_generator::encode_operand(const zydis_decode& instruction, zydis_dreg op_reg)
{
    const auto[displacement, size] = rm_.get_stack_displacement(op_reg.value);
    const vm_handler_entry& va_of_push_func = hg_.vm_handlers[2];
    const auto func_address = hg_.get_va_index(va_of_push_func, zydis_helper::get_reg_size(op_reg.value));

    //this routine will load the register value to the top of the VSTACK
    const std::vector load_register
        {
            //mov VTEMP, -8
            //call VM_LOAD_REG
            zydis_helper::create_encode_request<ZYDIS_MNEMONIC_MOV, zydis_ereg, zydis_eimm>(ZREG(VTEMP), ZIMMS(displacement)),
            zydis_helper::create_encode_request<ZYDIS_MNEMONIC_JMP, zydis_eimm>(ZIMMU(func_address)),
        };

    return {load_register, encode_status::success};
}

encode_data vm_generator::encode_operand(const zydis_decode& instruction, zydis_dmem op_mem)
{
    std::vector translated_mem
        {
            zydis_helper::create_encode_request<ZYDIS_MNEMONIC_SUB, zydis_ereg, zydis_eimm>(ZREG(VSP), ZIMMU(8))
        };

    return {translated_mem, encode_status::success};
}

encode_data vm_generator::encode_operand(const zydis_decode& instruction, zydis_dptr op_ptr)
{
    // ¯\_(ツ)_/¯
    return {{}, encode_status::unsupported};
}

encode_data vm_generator::encode_operand(const zydis_decode& instruction, zydis_dimm op_imm)
{
    auto imm = op_imm.value;
    const vm_handler_entry& va_of_push_func = hg_.vm_handlers[ZYDIS_MNEMONIC_PUSH];

    std::vector<zydis_encoder_request> translated_imm;

    const auto r_size = reg_size(instruction.operands[0].size);
    const auto func_address_mem = hg_.get_va_index(va_of_push_func, r_size);
    switch (r_size)
    {
        case bit64:
            translated_imm =
                {
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_MOV, zydis_ereg, zydis_eimm>(ZREG(VTEMP), ZIMMU(imm.u)),
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_JMP, zydis_eimm>(ZIMMU(func_address_mem))
                };
            break;
        case bit32:
            translated_imm =
                {
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_MOV, zydis_ereg, zydis_eimm>(ZREG(TO32(VTEMP)), ZIMMU(imm.u)),
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_JMP, zydis_eimm>(ZIMMU(func_address_mem))
                };
            break;
        case bit16:
            translated_imm =
                {
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_MOV, zydis_ereg, zydis_eimm>(ZREG(TO16(VTEMP)), ZIMMU(imm.u)),
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_JMP, zydis_eimm>(ZIMMU(func_address_mem))
                };
            break;
        case bit8:
            translated_imm =
                {
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_MOV, zydis_ereg, zydis_eimm>(ZREG(TO8(VTEMP)), ZIMMU(imm.u)),
                    zydis_helper::create_encode_request<ZYDIS_MNEMONIC_JMP, zydis_eimm>(ZIMMU(func_address_mem))
                };
            break;
        default:
            return {{}, encode_status::unsupported};
    }


    return {translated_imm, encode_status::success};
}