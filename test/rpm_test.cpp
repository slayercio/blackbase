#include <iostream>
#include <string>

int main()
{
    static const char* super_secret = "This is a super secret string!";
    std::cout << "Hello, World!" << std::endl;

    std::string user_input;
    std::cout << "Enter something: ";
    std::cin >> user_input;
    std::cout << "You entered: " << user_input << std::endl;

    if (user_input == "reveal") 
    {
        std::cout << "Revealing: " << super_secret << std::endl;
    }

    return 0;
}