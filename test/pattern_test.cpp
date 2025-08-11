#include <blackbase/pattern/pattern.hpp>
#include <iostream>

auto PrintBytes(const std::vector<std::uint8_t>& bytes)
{
    for (const auto& byte : bytes)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

auto TestPattern(const std::string_view& pattern, const std::string_view& mask)
{
    try
    {
        blackbase::Pattern p(pattern, mask);
        const auto& bytes = p.GetBytes();
        const auto& maskBytes = p.GetMask();

        std::cout << xorstr_("Pattern Bytes: ");
        PrintBytes(bytes);

        std::cout << xorstr_("Pattern Mask: ");
        PrintBytes(maskBytes);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

auto TestPatternCombo(const std::string_view& comboPattern)
{
    try
    {
        blackbase::Pattern p(comboPattern);
        const auto& bytes = p.GetBytes();
        const auto& maskBytes = p.GetMask();

        std::cout << xorstr_("Combo Pattern Bytes: ");
        PrintBytes(bytes);

        std::cout << xorstr_("Combo Pattern Mask: ");
        PrintBytes(maskBytes);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

int main()
{
    // Valid patterns and masks
    TestPattern(xorstr_("A1 B2 C3 00"), xorstr_("x x x ?"));
    TestPatternCombo(xorstr_("A1 B2 ? C3 ?"));
    TestPattern(xorstr_("FF 00 01 02"), xorstr_("x ? x x"));
    TestPatternCombo(xorstr_("FF ? 01 02 ?"));

    // Invalid patterns and masks
    TestPattern(xorstr_("A1 B2 C3"), xorstr_("x x")); // Should throw an error due to mismatched lengths
    TestPattern(xorstr_("FF 00 GG 02"), xorstr_("x x x x")); // Should throw an error due to invalid hex digit
    TestPatternCombo(xorstr_("A1 B2 C3 D4 ?"));

    // test with multiple whitespaces
    TestPatternCombo(xorstr_("  A1   B2   ?   C3   ?  "));
    TestPatternCombo(xorstr_("A1 B2 ? C3 ?  \nD4"));

    return 0;
}