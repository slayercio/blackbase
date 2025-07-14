#include <blackbase/blackbase.hpp>
#include <blackbase/pattern/pattern.hpp>
#include <iostream>

void test_pattern(const std::string_view& patternStr, const std::string_view& maskStr)
{
    blackbase::pattern::Pattern pattern(patternStr, maskStr);

    std::cout << "Pattern Bytes: ";
    for (const auto& byte : pattern.getBytes())
    {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    std::cout << "Pattern Mask: ";
    for (const auto& mask : pattern.getMask())
    {
        std::cout << (mask ? 'x' : '?') << " ";
    }
    std::cout << std::endl;
}

void test_combo_pattern(const std::string_view& comboStr)
{
    blackbase::pattern::Pattern pattern(comboStr);

    std::cout << "Combo Pattern Bytes: ";
    for (const auto& byte : pattern.getBytes())
    {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    std::cout << "Combo Pattern Mask: ";
    for (const auto& mask : pattern.getMask())
    {
        std::cout << (mask ? 'x' : '?') << " ";
    }
    std::cout << std::endl;
}

int main()
{
    test_pattern("A1 B2 C3", "x x x");
    test_pattern("A1B2C3", "xxx");
    test_pattern("FF 00 1A", "x ? x");
    test_pattern("A1 B2 C3 D4", "xxxx");
    test_pattern("A1B2C3D4", "x ? x x");
    test_pattern("A1 B2 C3 D4 E5", "x x x x x");
    // invalid patterns
    try
    {
        test_pattern("A1 B2 C3", "x x"); // Mismatched sizes
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    test_combo_pattern("A1 ? C3 ?");
    test_combo_pattern("FF ? 1A ? 2B");
    test_combo_pattern("A1B2 ? C3D4 ? E5F6");
    test_combo_pattern("A1 ? C3 ? D4 E5");
    test_combo_pattern("A1B2C3D4 ? E5F6 ?");
    test_combo_pattern("A1 ? C3 ? D4 E5 F6");
    test_combo_pattern("A1B2C3D4E5 ? F6 ?");
    try
    {
        test_combo_pattern("A1 ? C3 ? D4 E5 F6 G7"); // Invalid combo pattern
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}