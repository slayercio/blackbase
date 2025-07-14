#pragma once
#include <blackbase/disassembler/instruction.hpp>
#include <blackbase/detail/disassembler/zydis_status.hpp>
#include <Zydis/Zydis.h>

namespace blackbase::detail::disassembler 
{
    class ZydisInstruction : public blackbase::disassembler::IInstruction
    {
    private:
        std::uintptr_t m_Address;
        ZydisDecodedInstruction m_Instruction;
        std::vector<std::shared_ptr<blackbase::disassembler::IOperand>> m_Operands;
        std::string m_Text;

        inline static ZydisFormatter m_Formatter = []
        {
            ZydisFormatter fmt;
            ZyanStatus status = ZydisFormatterInit(&fmt, ZYDIS_FORMATTER_STYLE_INTEL);

            if (ZYAN_FAILED(status))
            {
                BLACKBASE_TRACE("Failed to initialize ZydisFormatter: {}", detail::disassembler::ZydisMessage::GetMessage(status));
            }

            return fmt;
        }();

    public:
        ZydisInstruction(std::uintptr_t address, const ZydisDecodedInstruction& instruction, 
                         const std::vector<std::shared_ptr<blackbase::disassembler::IOperand>>& operands, const std::string& text = "")
            : m_Address(address), m_Instruction(instruction), m_Operands(operands), m_Text(text)
        {
        }

        std::string GetMnemonic() const override
        {
            return ZydisMnemonicGetString(m_Instruction.mnemonic);
        }

        std::vector<std::shared_ptr<blackbase::disassembler::IOperand>> GetOperands() const override
        {
            return m_Operands;
        }

        std::string ToString() const override
        {   
            return m_Text;
        }

        std::size_t GetSize() const override
        {
            return m_Instruction.length;
        }

        std::uintptr_t GetAddress() const override
        {
            return m_Address;
        }

        std::string GetHex() const override
        {
            std::string hex;

            for (auto byte : GetRawBytes())
            {
                hex += std::format("{:02x} ", byte);
            }

            return hex;
        }

        std::vector<std::uint8_t> GetRawBytes() const override
        {
            return std::vector<std::uint8_t>(m_Address, m_Address + m_Instruction.length);
        }

        bool IsValid() const override
        {
            return m_Instruction.mnemonic != ZYDIS_MNEMONIC_INVALID;
        }

        bool IsBranch() const override
        {
            return m_Instruction.meta.category == ZYDIS_CATEGORY_COND_BR ||
                   m_Instruction.meta.category == ZYDIS_CATEGORY_UNCOND_BR;
        }

        bool IsCall() const override
        {
            return m_Instruction.meta.category == ZYDIS_CATEGORY_CALL;
        }

        bool IsJump() const override
        {
            return m_Instruction.meta.category == ZYDIS_CATEGORY_UNCOND_BR ||
                   m_Instruction.meta.category == ZYDIS_CATEGORY_COND_BR;
        }

        bool IsReturn() const override
        {
            return m_Instruction.mnemonic == ZYDIS_MNEMONIC_RET;
        }

        std::uint64_t GetTargetAddress() const override
        {
            for (const auto& operand : m_Operands)
            {
                if (operand->GetType() == blackbase::disassembler::IOperand::Type::Immediate)
                {
                    auto value = std::any_cast<ZydisDecodedOperandImm::ZydisDecodedOperandImmValue_>(operand->GetValue());

                    return value.u;
                }
                else if (operand->GetType() == blackbase::disassembler::IOperand::Type::Memory)
                {
                    auto raw = std::any_cast<ZydisDecodedOperand>(operand->GetNative());

                    if (raw.mem.base == ZYDIS_REGISTER_RIP)
                    {
                        return m_Address + m_Instruction.length + raw.mem.disp.value;
                    }
                }
            }

            return m_Address;
        }

        virtual ~ZydisInstruction() = default;
    };
}