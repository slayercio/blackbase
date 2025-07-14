#pragma once
#include <blackbase/internal/system.hpp>
#include <blackbase/library/library.hpp>

namespace blackbase::library
{
    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library()
        : m_Handle(0), m_Size(0), m_Name("") {}
    
    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library(std::uintptr_t handle, std::size_t size, const std::string_view& name)
        : m_Handle(handle), m_Size(size), m_Name(name) {}

    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library(const Library& other)
        : m_Handle(other.m_Handle), m_Size(other.m_Size), m_Name(other.m_Name) {}

    BLACKBASE_API BLACKBASE_CONSTEXPR Library& Library::operator=(const Library& other)
    {
        if (this != &other)
        {
            m_Handle = other.m_Handle;
            m_Size = other.m_Size;
            m_Name = other.m_Name;
        }
        return *this;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library(Library&& other) noexcept
        : m_Handle(other.m_Handle), m_Size(other.m_Size), m_Name(std::move(other.m_Name))
    {
        other.m_Handle = 0;
        other.m_Size = 0;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Library& Library::operator=(Library&& other) noexcept
    {
        if (this != &other)
        {
            m_Handle = other.m_Handle;
            m_Size = other.m_Size;
            m_Name = std::move(other.m_Name);

            other.m_Handle = 0;
            other.m_Size = 0;
        }

        return *this;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t Library::GetHandle() const
    {
        return m_Handle;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR const std::string& Library::GetName() const
    {
        return m_Name;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::size_t Library::GetSize() const
    {
        return m_Size;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR bool Library::IsValid() const
    {
        return m_Handle != 0;
    }
}

#if defined(BLACKBASE_SYSTEM_WINDOWS)
    #include <blackbase/detail/library/library_impl_win.hpp>
#elif defined(BLACKBASE_SYSTEM_LINUX)
    #include <blackbase/detail/library/library_impl_linux.hpp>
#elif defined(BLACKBASE_SYSTEM_MACOS)
    #include <blackbase/detail/library/library_impl_macos.hpp>
#else
    #error "Unsupported system for library implementation"
#endif