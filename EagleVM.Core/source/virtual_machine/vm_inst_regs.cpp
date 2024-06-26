#include "eaglevm-core/virtual_machine/vm_inst_regs.h"
#include "eaglevm-core/util/random.h"

vm_inst_regs::vm_inst_regs()
{
    reg_stack_order_ = {};
    reg_vm_order_ = {};
}

void vm_inst_regs::init_reg_order()
{
    for(int i = ZYDIS_REGISTER_RAX; i <= ZYDIS_REGISTER_R15; i++)
        reg_stack_order_[i - ZYDIS_REGISTER_RAX] = static_cast<zydis_register>(i);

    std::ranges::shuffle(reg_stack_order_, ran_device::get().gen);

    bool success_shuffle = false;
    std::array<zydis_register, 16> access_order = reg_stack_order_;

    while(!success_shuffle)
    {
        std::ranges::shuffle(access_order, ran_device::get().gen);

        success_shuffle = true;
        for(int i = 0; i < NUM_OF_VREGS; i++)
        {
            const zydis_register target = access_order[i];

            // these are registers we do not want to be using as VM registers
            if(target == ZYDIS_REGISTER_RIP || target == ZYDIS_REGISTER_RAX || target == ZYDIS_REGISTER_RSP)
            {
                success_shuffle = false;
                break;
            }
        }
    }

    reg_vm_order_ = access_order;
}

zydis_register vm_inst_regs::get_reg(const uint8_t target) const
{
    // this would be something like VIP, VSP, VTEMP, etc
    if(target > NUM_OF_VREGS - 1 )
        __debugbreak();

    return reg_vm_order_[target];
}

std::pair<uint32_t, reg_size> vm_inst_regs::get_stack_displacement(const zydis_register reg) const
{
    //determine 64bit version of register
    reg_size reg_size = zydis_helper::get_reg_size(reg);
    zydis_register bit64_reg = zydis_helper::get_bit_version(reg, bit64);

    int found_index = 0;
    const auto reg_count = reg_stack_order_.size();

    for(int i = 0; i < reg_count; i++)
    {
        if(bit64_reg == reg_stack_order_[i])
        {
            found_index = i;
            break;
        }
    }

    int offset = 0;
    if(reg_size == bit8)
    {
        if(zydis_helper::is_upper_8(reg))
            offset += 1;
    }

    return { found_index * 8 + offset, reg_size };
}

void vm_inst_regs::enumerate(const std::function<void(zydis_register)>& enumerable, bool from_back)
{
    if(from_back)
        std::ranges::for_each(reg_stack_order_, enumerable);
    else
        std::for_each(reg_stack_order_.rbegin(), reg_stack_order_.rend(), enumerable);
}
