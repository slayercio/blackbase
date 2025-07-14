#pragma once
#ifdef BLACKBASE_ENABLE_DISASSEMBLER
#include <blackbase/common.hpp>
#include <blackbase/disassembler/operand.hpp>

namespace blackbase::disassembler
{
    struct IInstruction
    {
        /**
         * Returns the mnemonic of the instruction.
         * The mnemonic is a human-readable representation of the instruction operation (e.g., "MOV", "ADD").
         */
        BLACKBASE_API virtual std::string GetMnemonic() const = 0;
        
        /**
         * Returns a vector of operands associated with the instruction.
         * Each operand is represented by a shared pointer to an IOperand interface.
         */
        BLACKBASE_API virtual std::vector<std::shared_ptr<IOperand>> GetOperands() const = 0;
        
        /**
         * Returns a string representation of the instruction.
         * This includes the mnemonic and operands formatted as a human-readable string.
         */
        BLACKBASE_API virtual std::string ToString() const = 0;

        /**
         * Returns the size of the instruction in bytes.
         * This is the total number of bytes that the instruction occupies in memory.
         */
        BLACKBASE_API virtual std::size_t GetSize() const = 0;

        /**
         * Returns the address of the instruction in memory.
         * This is typically the address where the instruction is located in the disassembled binary.
         */
        BLACKBASE_API virtual std::uint64_t GetAddress() const = 0;
        
        /**
         * Returns the hexadecimal representation of the instruction.
         * This is he raw bytes of the instruction formatted as a string of hexadecimal digits.
         */
        BLACKBASE_API virtual std::string GetHex() const = 0;
        
        /**
         * Returns the raw bytes of the instruction as a vector of bytes.
         * This is useful for low-level operations or when you need to manipulate the instruction bytes directly.
         */
        BLACKBASE_API virtual std::vector<std::uint8_t> GetRawBytes() const = 0;

        /**
         * Returns true if the instruction is valid.
         * A valid instruction is one that has been successfully disassembled and contains meaningful data.
         */
        BLACKBASE_API virtual bool IsValid() const = 0;

        /**
         * Returns true if the instruction is a branch instruction (e.g., conditional jumps, calls, or jumps).
         */
        BLACKBASE_API virtual bool IsBranch() const = 0;

        /**
         * Returns true if the instruction is a call instruction (e.g., CALL).
         * This is distinct from branch instructions, which may include jumps.
         */
        BLACKBASE_API virtual bool IsCall() const = 0;

        /**
         * Returns true if the instruction is a jump instruction (e.g., JMP).
         * This is distinct from branch instructions, which may include conditional jumps.
         */
        BLACKBASE_API virtual bool IsJump() const = 0;

        /**
         * Returns true if the instruction is a return instruction (e.g., RET).
         */
        BLACKBASE_API virtual bool IsReturn() const = 0;

        /**
         * Returns the target address for branch, call, or jump instructions.
         * For other instructions, it returns the address of the instruction itself.
         */
        BLACKBASE_API virtual std::uint64_t GetTargetAddress() const = 0;

        /**
         * Destructor for the instruction interface.
         * It is virtual to allow derived classes to clean up resources properly.
         */
        BLACKBASE_API virtual ~IInstruction() = default;
    };
}
#endif