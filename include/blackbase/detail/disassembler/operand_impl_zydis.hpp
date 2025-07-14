#pragma once
#include <blackbase/disassembler/operand.hpp>
#include <blackbase/internal/constexpr.hpp>
#include <blackbase/internal/log.hpp>
#include <blackbase/detail/disassembler/zydis_status.hpp>

#include <Zydis/Zydis.h>

namespace blackbase::detail::disassembler
{
    class ZydisOperand : public blackbase::disassembler::IOperand
    {
    private:
        ZydisDecodedOperand m_Operand;
        ZydisDecodedInstruction m_Instruction;
        std::uintptr_t m_Address;

        inline static ZydisFormatter m_Formatter = []
        {
            ZydisFormatter fmt;
            ZyanStatus status = ZydisFormatterInit(&fmt, ZYDIS_FORMATTER_STYLE_INTEL);

            if (status != ZYAN_STATUS_SUCCESS)
            {
                BLACKBASE_TRACE("Failed to initialize ZydisFormatter: {}", detail::disassembler::ZydisMessage::GetMessage(status));
            }

            return fmt;
        }();

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR ZydisOperand(const ZydisDecodedOperand& operand, const ZydisDecodedInstruction& instruction, std::uintptr_t address)
            : m_Operand(operand), m_Instruction(instruction), m_Address(address)
        {
        }

        BLACKBASE_API blackbase::disassembler::IOperand::Type GetType() const override
        {
            switch (m_Operand.type)
            {
                case ZYDIS_OPERAND_TYPE_REGISTER:
                    return blackbase::disassembler::IOperand::Type::Register;
                case ZYDIS_OPERAND_TYPE_IMMEDIATE:
                    return blackbase::disassembler::IOperand::Type::Immediate;
                case ZYDIS_OPERAND_TYPE_MEMORY:
                    return blackbase::disassembler::IOperand::Type::Memory;
                case ZYDIS_OPERAND_TYPE_POINTER:
                    return blackbase::disassembler::IOperand::Type::Pointer;
                case ZYDIS_OPERAND_TYPE_UNUSED:
                default:
                    return blackbase::disassembler::IOperand::Type::Unknown;
            }
        }

        BLACKBASE_API std::string ToString() const override
        {
            char buffer[256];

            auto status = ZydisFormatterFormatOperand(
                &m_Formatter, &m_Instruction, &m_Operand, buffer, sizeof(buffer), m_Address, ZYAN_NULL
            );

            if (ZYAN_SUCCESS(status))
            {
                return std::string(buffer);
            }
            else
            {
                BLACKBASE_DEBUG("Failed to format operand: {}", detail::disassembler::ZydisMessage::GetMessage(status));
                return "Error formatting operand";
            }
        }

        BLACKBASE_API std::size_t GetSize() const override
        {
            return m_Operand.size / 8; // Convert bits to bytes
        }

        BLACKBASE_API std::any GetValue() const override
        {
            switch (m_Operand.type)
            {
                case ZYDIS_OPERAND_TYPE_REGISTER:
                    return m_Operand.reg.value; // Assuming reg.value is the register value
                case ZYDIS_OPERAND_TYPE_IMMEDIATE:
                    return m_Operand.imm.value; // Assuming imm.value is the immediate value
                case ZYDIS_OPERAND_TYPE_MEMORY:
                    return m_Operand.mem.base; // Assuming mem.base is the memory address
                case ZYDIS_OPERAND_TYPE_POINTER:
                    return m_Operand.ptr.segment; // Assuming ptr.segment is the pointer segment
                default:
                    return std::any(); // Unknown type, return empty any
            }
        }

        BLACKBASE_API std::any GetNative() const override
        {
            return std::any(m_Operand);
        }

        BLACKBASE_API ~ZydisOperand() override = default;
    };
}