#pragma once
#include <blackbase/common.hpp>

namespace blackbase
{
    template<typename Ret, typename... Args>
    struct FunctionCaller
    {
        using FuncType = Ret(*)(Args...);

        static Ret Call(std::uintptr_t address, Args... args)
        {
            FuncType func = reinterpret_cast<FuncType>(address);
            return func(std::forward<Args>(args)...);
        }
    };

    class Export
    {
    private:
        std::string     m_Name;
        std::uintptr_t  m_Address;

    public:
        inline Export(const std::string& name, std::uintptr_t address)
        {
            m_Name    = name;
            m_Address = address;
        }

    public:
        inline const std::string& GetName() const
        {
            return m_Name;
        }

        inline std::uintptr_t GetAddress() const
        {
            return m_Address;
        }

    public:
        template<typename Ret, typename... Args>
        inline Ret Call(Args... args) const
        {
            return FunctionCaller<Ret, Args...>::Call(m_Address, std::forward<Args>(args)...);
        }
    };
}