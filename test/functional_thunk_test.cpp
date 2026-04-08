#include <blackbase/functional.hpp>
#include <blackbase/function_inl_win.hpp>
#include <iostream>
#include <cstddef>

using callback_t = void(*)(size_t&);
void dispatch_callback(callback_t cb)
{
    static size_t counter = 0;
    cb(counter);
}

template<blackbase::functional::Allocator A>
void test_function_wrapper()
{
    for (int i = 0; i < 150000; ++i)
    {
        auto wrapped = blackbase::functional::FunctionWrapperImpl<A>::template wrap<void, size_t&>([](size_t& count)
        {
            ++count;
        });
    
        dispatch_callback(wrapped);
    }
}

int main()
{
    int selection = 0;
    std::cout << "Select allocator to test:\n1. Default ThunkAllocator\n2. PagedThunkAllocator\n> ";
    std::cin >> selection;

    switch (selection)
    {
        case 1:
            std::cout << "Testing FunctionWrapper with default allocator...\n";
            test_function_wrapper<blackbase::functional::ThunkAllocator>();
            break;

        case 2:
            std::cout << "Testing FunctionWrapper with paged allocator...\n";
            test_function_wrapper<blackbase::functional::PagedThunkAllocator>();
            break;

        default:
            std::cout << "Invalid selection.\n";
            break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}