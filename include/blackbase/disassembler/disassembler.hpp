#pragma once
#ifdef BLACKBASE_ENABLE_DISASSEMBLER

#include <blackbase/common.hpp>
#include <blackbase/internal/constexpr.hpp>
#include <blackbase/disassembler/operand.hpp>
#include <blackbase/disassembler/instruction.hpp>

namespace blackbase::disassembler
{
    class IDisassembler
    {
    public:
        BLACKBASE_API virtual ~IDisassembler() = default;

        /**
         * Disassembles a single instruction from the provided byte data.
         * This function processes the data and returns a single disassembled instruction.
         * @param data Pointer to the byte data to disassemble.
         * @param size The size of the data in bytes.
         * @param address The starting address of the data in memory, used for instruction addressing.
         * @return An optional IInstruction object representing the disassembled instruction, or std::nullopt if disassembly fails.
         */
        BLACKBASE_API virtual std::optional<std::shared_ptr<IInstruction>> DisassembleInstruction(
            std::uint8_t* data,
            std::size_t size,
            std::uint64_t address = 0
        ) = 0;

        /**
         * Disassembles a sequence of bytes into a vector of instructions.
         * This function processes the provided data and returns a vector of disassembled instructions.
         * @param data Pointer to the byte data to disassemble.
         * @param size The size of the data in bytes.
         * @param address The starting address of the data in memory, used for instruction addressing.
         * @return A vector of disassembled instructions. Each instruction is represented by an IInstruction
         */
        BLACKBASE_API virtual std::vector<std::shared_ptr<IInstruction>> Disassemble(
            std::uint8_t* data,
            std::size_t size,
            std::uint64_t address = 0
        ) = 0;

        /**
         * Returns the name of the disassembler implementation.
         * This is a string that identifies the disassembler, such as "Zydis", "Udis86"
         */
        BLACKBASE_API virtual const std::string& GetImplementationName() const = 0;
    };

    BLACKBASE_API IDisassembler* CreateDisassembler();
}

#endif