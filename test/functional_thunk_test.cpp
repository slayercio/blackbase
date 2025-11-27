#include <blackbase/functional.hpp>
#include <blackbase/function_inl_win.hpp>
#include <iostream>

using callback_t = void(*)(size_t&);
void dispatch_callback(callback_t cb)
{
    static size_t counter = 0;
    cb(counter);
}

int main()
{
    auto wrapped = blackbase::functional::FunctionWrapper::wrap<void, size_t&>([](size_t& count)
    {
        ++count;
        std::cout << "Callback invoked. Current count: " << count << "\n";
    });

    for (int i = 0; i < 5; ++i)
    {
        dispatch_callback(wrapped);
    }
}