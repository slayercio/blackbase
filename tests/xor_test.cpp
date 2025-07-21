#include <blackbase/xor/xor_string.hpp>
#include <print>

auto key = 0x11223344;

template<auto T>
struct EnsureCompiled
{
    constexpr static auto value = T;
};

int main()
{
    auto x = EnsureCompiled<10>::value;
    
    auto y = &x;
    std::print("x = {}\n", *y);
    std::print("y = {:#018x}\n", (std::uint64_t) y);

    auto xorpattern = xorstr("48 8D 0D ? ? ? ?");
    std::print("xor_pattern = {}\n", xorpattern.crypt_get());
}