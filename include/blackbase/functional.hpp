#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
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

    struct ThunkAllocator
    {
        using deleter_t = void(*)(void*);

        static void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t));
        static void deallocate(void* stub_ptr, std::size_t size);
        static void track(void* this_ptr, void* stub_ptr, std::size_t size, bool auto_delete, deleter_t deleter = nullptr);
        static void finish(void* stub_ptr, std::size_t size);
    };

    template<typename A>
    concept Allocator = requires(A a, std::size_t size)
    {
        { A::allocate(size) } -> std::same_as<void*>;
        { A::deallocate(std::declval<void*>(), size) } -> std::same_as<void>;
        { A::track(std::declval<void*>(), std::declval<void*>(), size, std::declval<bool>(), std::declval<typename A::deleter_t>()) } -> std::same_as<void>;
        { A::finish(std::declval<void*>(), size) } -> std::same_as<void>;
    };

    template<Allocator A>
    class FunctionWrapperImpl
    {
    private:
        inline static void* create_thunk(void* this_ptr, void* dispatch_ptr, typename A::deleter_t deleter, bool auto_delete)
        {
            constexpr std::size_t thunk_size = 32;
            void* stub = A::allocate(thunk_size);
            if (!stub)
            {
                return nullptr;
            }

            #pragma pack(push, 1)
            struct ThunkCode
            {
                uint8_t mov_r10[2] = { 0x49, 0xBA }; // mov r10, imm64
                void* this_ptr;
                uint8_t mov_rax[2] = { 0x48, 0xB8 }; // mov rax, imm64
                void* dispatch_ptr;
                uint8_t jmp_rax[2] = { 0xFF, 0xE0 }; // jmp rax
            };
            #pragma pack(pop)

            ThunkCode code{};
            code.this_ptr = this_ptr;
            code.dispatch_ptr = dispatch_ptr;
            std::memcpy(stub, &code, sizeof(ThunkCode));

            A::finish(stub, thunk_size);

            return stub;
        }

        inline static void* read_r10()
        {
            using read_r10_t = void* (*)();
            
            static read_r10_t stub = []() -> read_r10_t
            {
                auto stub = A::allocate(4);
                if (!stub)
                {
                    return nullptr;
                }

                #pragma pack(push, 1)
                struct ReadR10Code
                {
                    uint8_t mov_rax_r10[3] = { 0x4C, 0x8B, 0xD0 }; // mov rax, r10
                    uint8_t ret[1] = { 0xC3 }; // ret
                };
                #pragma pack(pop)

                ReadR10Code code;
                std::memcpy(stub, &code, sizeof(ReadR10Code));

                A::finish(stub, 4);
                return reinterpret_cast<read_r10_t>(stub);
            }();

            return stub();
        }

    public:
        template<typename R, typename... Args, typename F>
        static typename function_traits<R(Args...)>::ptr wrap(F&& f, bool auto_delete = true)
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
            auto stub = create_thunk(wrapper, reinterpret_cast<void*>(&thunk_generator::call), &thunk_generator::deleter, auto_delete);
            A::track(wrapper, stub, 32, auto_delete, &thunk_generator::deleter);

            return reinterpret_cast<typename function_traits<R(Args...)>::ptr>(stub);
        }
    };

    using FunctionWrapper = FunctionWrapperImpl<ThunkAllocator>;
}