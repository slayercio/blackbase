#pragma once
#include <string>
#include <blackbase/xorstr.hpp>

// this impl to force macro expansion of value before passing to xorstr_
#define OBFUSCATED_VALUE_IMPL(value, base) std::strtoull(xorstr_(#value), nullptr, base)
#define OBFUSCATED_VALUE_IMPL_S(value, base) std::strtoll(xorstr_(#value), nullptr, base)

#define OBFUSCATED_VALUE(value) OBFUSCATED_VALUE_IMPL(value, 10)
#define OBFUSCATED_HVALUE(value) OBFUSCATED_VALUE_IMPL(value, 16)

#define OBFUSCATED_SVALUE(value) OBFUSCATED_VALUE_IMPL_S(value, 10)
#define OBFUSCATED_SHVALUE(value) OBFUSCATED_VALUE_IMPL_S(value, 16)

#define OBFUSCATED_ZERO OBFUSCATED_VALUE(0)
#define OBFUSCATED_ONE OBFUSCATED_VALUE(1)

#define OBFUSCATED_TRUE static_cast<bool>(OBFUSCATED_ONE)
#define OBFUSCATED_FALSE static_cast<bool>(OBFUSCATED_ZERO)

#define OBFUSCATED_NULLPTR(type) reinterpret_cast<type*>(OBFUSCATED_ZERO)