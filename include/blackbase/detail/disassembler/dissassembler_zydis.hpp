#pragma once
#include <blackbase/disassembler/disassembler.hpp>
#include <blackbase/detail/disassembler/operand_impl_zydis.hpp>
#include <blackbase/detail/disassembler/instruction_impl_zydis.hpp>

#include <Zydis/Zydis.h>

namespace blackbase
{
    namespace detail::disassembler
    {
        class ZydisDisassembler : public blackbase::disassembler::IDisassembler
        {
        private:
            ZydisDecoder m_Decoder;
            ZydisFormatter m_Formatter;

        public:
            ZydisDisassembler()
            {
                ZydisDecoderInit(&m_Decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
                ZydisFormatterInit(&m_Formatter, ZYDIS_FORMATTER_STYLE_INTEL);
            }

            std::optional<std::shared_ptr<blackbase::disassembler::IInstruction>> DisassembleInstruction(
                std::uint8_t* data,
                std::size_t size,
                std::uint64_t address
            ) override
            {
                ZydisDecodedInstruction instruction;
                ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

                ZyanStatus status = ZydisDecoderDecodeFull(&m_Decoder, data, size, &instruction, operands);
                if (ZYAN_FAILED(status))
                {
                    return std::nullopt; // Disassembly failed
                }

                char buffer[256];

                status = ZydisFormatterFormatInstruction(
                    &m_Formatter, &instruction, operands, instruction.operand_count_visible, buffer, sizeof(buffer), address, nullptr 
                );
                if (ZYAN_FAILED(status))
                {
                    BLACKBASE_DEBUG("Failed to format instruction: {}", detail::disassembler::ZydisMessage::GetMessage(status));
                    return std::nullopt; // Formatting failed
                }

                std::vector<std::shared_ptr<blackbase::disassembler::IOperand>> operandList;
                for (auto i = 0; i < instruction.operand_count_visible; ++i)
                {
                    operandList.push_back(
                        std::make_shared<detail::disassembler::ZydisOperand>(
                            operands[i], instruction, address
                        )
                    );
                }

                return std::make_shared<detail::disassembler::ZydisInstruction>(
                    address, instruction, operandList, std::string(buffer)
                );
            }

            std::vector<std::shared_ptr<blackbase::disassembler::IInstruction>> Disassemble(
                std::uint8_t* data,
                std::size_t size,
                std::uint64_t address
            ) override
            {
                std::vector<std::shared_ptr<blackbase::disassembler::IInstruction>> instructions;

                std::size_t offset = 0; 

                while (offset < size)
                {
                    auto instruction = DisassembleInstruction(data + offset, size - offset, address + offset);

                    if (!instruction.has_value())
                    {
                        BLACKBASE_DEBUG("Failed to disassemble instruction at offset {}", offset);
                        break; // Stop if disassembly fails
                    }

                    offset += instruction.value()->GetSize();
                    instructions.push_back(instruction.value());
                }

                return instructions;
            }

            const std::string& GetImplementationName() const override
            {
                static const std::string name = "Zydis";
                return name;
            }

            BLACKBASE_API ~ZydisDisassembler() override = default;
        };
    }

    BLACKBASE_API disassembler::IDisassembler* disassembler::CreateDisassembler()
    {
        static detail::disassembler::ZydisDisassembler disassembler;

        return &disassembler;
    }


} // namespace blackbase::detail::disassembler
