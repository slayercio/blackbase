#pragma once
#include <string>
#include <blackbase/xorstr.hpp>

// this impl to force macro expansion of value before passing to xorstr_
#define OBFUSCATED_VALUE_IMPL(value, base) std::strtoull(xorstr_(#value), nullptr, base)

#define OBFUSCATED_VALUE(value) OBFUSCATED_VALUE_IMPL(value, 10)
#define OBFUSCATED_HVALUE(value) OBFUSCATED_VALUE_IMPL(value, 16)

#define OBFUSCATED_TRUE static_cast<bool>(OBFUSCATED_VALUE(1))
#define OBFUSCATED_FALSE static_cast<bool>(OBFUSCATED_VALUE(0))