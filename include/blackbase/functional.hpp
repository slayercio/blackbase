#pragma once
#include <cstdint>
#include <cstddef>
#include <tuple>

namespace blackbase::functional
{
    template<typename F>
    struct function_traits;

    template<typename R, typename... Args>
    struct function_traits<R(Args...)>
    {
        using return_type = R;
        using ptr = R(*)(Args...);
        using argument_types = std::tuple<Args...>;

        static constexpr std::size_t arity = sizeof...(Args);
    };

    template<typename R, typename... Args>
    struct function_traits<R(*const)(Args...)> : function_traits<R(Args...)> {};

    template<typename C, typename R, typename... Args>
    struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> {};

    template<typename F>
    struct function_traits : function_traits<decltype(&F::operator())> {};

    template<typename Tuple, typename F, std::size_t... I>
    void for_each_type_impl(F&& f, std::index_sequence<I...>)
    {
        (f.template operator()<std::tuple_element_t<I, Tuple>>(), ...);
    }

    template<typename Tuple, typename F>
    void for_each_type(F&& f)
    {
        constexpr std::size_t N = std::tuple_size_v<Tuple>;
        for_each_type_impl<Tuple>(
            std::forward<F>(f),
            std::make_index_sequence<N>{}
        );
    }

    // wraps std::function into a function ptr
    class FunctionWrapper
    {
    private:
        using deleter_t = void(*)(void*);

        static void* create_thunk(void* this_ptr, void* dispatch_ptr, deleter_t deleter);
        static void* read_r10();

    public:
        template<typename R, typename... Args, typename F>
        static typename function_traits<R(Args...)>::ptr wrap(F&& f)
        {
            struct thunk_generator
            {
            private:
                F func;

            public:
                static R call(Args... args)
                {
                    auto _this = reinterpret_cast<thunk_generator*>(read_r10());
                    return _this->func(std::forward<Args>(args)...);
                }

                static void deleter(void* ptr)
                {
                    delete reinterpret_cast<thunk_generator*>(ptr);
                }

                thunk_generator(F&& f) : func(std::forward<F>(f))
                {
                }
            };

            auto wrapper = new thunk_generator(std::forward<F>(f));
            auto stub = create_thunk(wrapper, reinterpret_cast<void*>(&thunk_generator::call), &thunk_generator::deleter);
            return reinterpret_cast<typename function_traits<R(Args...)>::ptr>(stub);
        }
    };
}