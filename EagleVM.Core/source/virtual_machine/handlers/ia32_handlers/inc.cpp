#include "eaglevm-core/virtual_machine/handlers/ia32_handlers/inc.h"

void ia32_inc_handler::construct_single(function_container& container, reg_size reg_size, uint8_t operands)
{
    const vm_handler_entry* push_rflags_handler = hg_->v_handlers[MNEMONIC_VM_PUSH_RFLAGS];
    const inst_handler_entry* pop_handler = hg_->inst_handlers[ZYDIS_MNEMONIC_POP];

    // pop VTEMP                    ; VTEMP should be an address (always 64 bits)
    call_vm_handler(container, pop_handler->get_handler_va(bit64, 1));

    //inc [VTEMP]                   ; inc popped value
    container.add(zydis_helper::enc(ZYDIS_MNEMONIC_INC, ZMEMBD(VTEMP, 0, reg_size)));

    //pushfq                        ; keep track of rflags
    call_vm_handler(container, push_rflags_handler->get_vm_handler_va(bit64));

    create_vm_return(container);
}

void ia32_inc_handler::finalize_translate_to_virtual(const zydis_decode& decoded_instruction, function_container& container)
{
    inst_handler_entry::finalize_translate_to_virtual(decoded_instruction, container);

    // accept changes to rflags
    const vm_handler_entry* store_handler = hg_->v_handlers[MNEMONIC_VM_POP_RFLAGS];
    call_vm_handler(container, store_handler->get_vm_handler_va(bit64));
}

bool ia32_inc_handler::virtualize_as_address(const zydis_decode& inst, int index)
{
    return index == 0;
}
