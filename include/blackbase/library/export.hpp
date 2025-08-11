#pragma once
#include <blackbase/common.hpp>

namespace blackbase
{
    class Export
    {
    private:
        std::string     m_Name;
        std::uintptr_t  m_Address;

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Export(const std::string& name, std::uintptr_t address);

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::string& GetName() const;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t GetAddress() const;

    public:
        template<typename Ret, typename... Args>
        BLACKBASE_FORCEINLINE Ret Call(Args... args) const
        {
            using FuncType = Ret(*)(Args...);
            FuncType func = reinterpret_cast<FuncType>(m_Address);
            return func(std::forward<Args>(args)...);
        }
    };
}

#pragma region Export Implementation
namespace blackbase
{
    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Export::Export(const std::string& name, std::uintptr_t address)
        : m_Name(name), m_Address(address)
    {
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::string& Export::GetName() const
    {
        return m_Name;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t Export::GetAddress() const
    {
        return m_Address;
    }
}
#pragma endregion