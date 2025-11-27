#include <blackbase/functional.hpp>
#include <iostream>

template<typename R, typename... Args>
void print_info_about_function(R(*f)(Args...))
{
    using traits = blackbase::functional::function_traits<R(Args...)>;
    std::cout << "Function arity: " << traits::arity << "\n";
    std::cout << "Return type: " << typeid(typename traits::return_type).name() << "\n";

    using args = typename traits::argument_types;
    std::cout << "Argument types: ";

    blackbase::functional::for_each_type<args>([]<typename T>()
    {
        std::cout << "[" << typeid(T).name() << "] ";
    });
    std::cout << "\n";
}

template<typename F>
void print_info_about_callable(F f)
{
    using traits = blackbase::functional::function_traits<F>;
    std::cout << "Callable arity: " << traits::arity << "\n";
    std::cout << "Return type: " << typeid(typename traits::return_type).name() << "\n";

    using args = typename traits::argument_types;
    std::cout << "Argument types: ";

    blackbase::functional::for_each_type<args>([]<typename T>()
    {
        std::cout << "[" << typeid(T).name() << "] ";
    });
    std::cout << "\n";
}

int add(int a, int b)
{
    return a + b;
}

int many_args(int a, int b, int c, int d, int e, double f, const std::string& g)
{
    return a + b + c + d + e + f;
}

int main()
{
    print_info_about_function(add);
    print_info_about_function(many_args);

    struct Lambda {
        int operator()(int x, double y) { return x + static_cast<int>(y); }
    };
    print_info_about_callable(Lambda{});
}