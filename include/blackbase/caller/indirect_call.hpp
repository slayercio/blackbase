#pragma once
#include <blackbase/common.hpp>
#include <type_traits>
#include <utility>

extern "C" void* _call_stub();

namespace blackbase::caller 
{
    class IndirectCall
    {
    public:
        template<typename Ret, typename... Args>
        static inline auto call_helper(const void* shellcode, Args... args) -> Ret
        {
            auto fn = (Ret(*)(Args...))(shellcode);
            return fn(args...);
        }

        template<std::size_t N, typename>
        struct argument_remapper
        {
            template<typename Ret, typename First, typename Second, typename Third, typename Fourth, typename... Rest>
            static auto do_call(
                const void* shellcode, void* shell_param, First first, Second second, Third third, Fourth fourth, Rest... rest
            ) -> Ret
            {
                return call_helper<Ret, First, Second, Third, Fourth, void*, void*, Rest...>(
                    shellcode, first, second, third, fourth, shell_param, nullptr, rest...
                );
            }
        };

        template<std::size_t N>
        struct argument_remapper<N, std::enable_if_t<N <= 4>>
        {
            template<typename Ret, typename First = void*, typename Second = void*, typename Third = void*, typename Fourth = void*>
            static auto do_call(
                const void* shellcode, void* shell_param, First first = First{}, Second second = Second{}, Third third = Third{}, Fourth fourth = Fourth{}
            ) -> Ret
            {
                return call_helper<Ret, First, Second, Third, Fourth, void*, void*>(
                    shellcode, first, second, third, fourth, shell_param, nullptr
                );
            }
        };

        template<typename Ret, typename... Args>
        static inline auto call(const void* trampoline, Ret(*fn)(Args...), Args... args) -> Ret
        {
            struct shell_params
            {
                const void* trampoline;
                void* function;
                void* rbx;
            };

            shell_params p { trampoline, (void*)fn };
            using mapper = argument_remapper<sizeof...(Args), void>;
            return mapper::template do_call<Ret, Args...>((const void*)_call_stub, &p, args...);
        }
    };
}