#pragma once
#include <string>
#include <blackbase/xorstr.hpp>

#define OBFUSCATED_VALUE(value) std::strtoull(xorstr_(#value), nullptr, 10)
#define OBFUSCATED_HVALUE(value) std::strtoull(xorstr_(#value), nullptr, 16)

#define OBFUSCATED_TRUE static_cast<bool>(OBFUSCATED_VALUE(1))
#define OBFUSCATED_FALSE static_cast<bool>(OBFUSCATED_VALUE(0))