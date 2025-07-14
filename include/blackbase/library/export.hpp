#pragma once
#include <blackbase/common.hpp>

namespace blackbase::library
{
    class Export
    {
    private:
        std::string m_Name;
        void*       m_Address;

    public:
        BLACKBASE_API Export(const std::string_view& name, void* address);
        BLACKBASE_API Export(const Export&);
        BLACKBASE_API Export& operator=(const Export&);

        BLACKBASE_API Export(Export&&) noexcept;
        BLACKBASE_API Export& operator=(Export&&) noexcept;

    public:
        BLACKBASE_API const std::string& GetName() const;
        BLACKBASE_API void* GetAddress() const;

    public:
        template<typename Ret, typename... Args>
        BLACKBASE_IMPL Ret Call(Args... args) const
        {
            using FuncType = Ret(*)(Args...);
            FuncType func = reinterpret_cast<FuncType>(m_Address);
            return func(std::forward<Args>(args)...);
        }
    };
}