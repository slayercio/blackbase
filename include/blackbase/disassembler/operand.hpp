#pragma once
#ifdef BLACKBASE_ENABLE_DISASSEMBLER

#include <blackbase/common.hpp>

namespace blackbase::disassembler
{
    struct IOperand
    {
        /**
         * Returns the type of the operand.
         * The type indicates whether the operand is a register, immediate value, memory address, or unknown.
         */
        enum class Type
        {
            Register,
            Immediate,
            Memory,
            Pointer,
            Unknown
        };

        /**
         * Returns the type of the operand.
         * This method should return one of the types defined in the Type enum.
         */
        BLACKBASE_API virtual IOperand::Type GetType() const = 0;
        
        /**
         * Returns the string representation of the operand.
         */
        BLACKBASE_API virtual std::string   ToString() const = 0;

        /**
         * Returns the size of the operand in bytes.
         * This is typically the size of the data represented by the operand (e.g., 4 bytes for a 32-bit integer).
         */
        BLACKBASE_API virtual std::size_t GetSize() const = 0;

        /**
         * Returns the value of the operand.
         * The value is returned as a std::any type, allowing for different types of values (e.g., integers, addresses).
         * The caller should check the type of the value before using it.
         */
        BLACKBASE_API virtual std::any GetValue() const = 0;


        /**
         * Returns the native representation of the operand.
         * This method is intended to return the underlying representation of the operand
         * as used by the disassembler library (e.g., Zydis -> ZydisDecodedOperand).
         */
        BLACKBASE_API virtual std::any GetNative() const = 0;

        /**
         * Returns the size of the operand in bytes.
         */
        BLACKBASE_API virtual ~IOperand() = default;
    };   
}

#endif