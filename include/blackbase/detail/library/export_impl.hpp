#pragma once
#include <blackbase/library/export.hpp>

namespace blackbase::library
{
    BLACKBASE_API Export::Export(const std::string_view& name, void* address)
        : m_Name(name), m_Address(address)
    {
    }

    BLACKBASE_API Export::Export(const Export& other)
        : m_Name(other.m_Name), m_Address(other.m_Address)
    {
    }

    BLACKBASE_API Export& Export::operator=(const Export& other)
    {
        if (this != &other)
        {
            m_Name = other.m_Name;
            m_Address = other.m_Address;
        }
        return *this;
    }

    BLACKBASE_API Export::Export(Export&& other) noexcept
        : m_Name(std::move(other.m_Name)), m_Address(other.m_Address)
    {
        other.m_Address = nullptr; // Ensure the moved-from object has a null address
    }

    BLACKBASE_API Export& Export::operator=(Export&& other) noexcept
    {
        if (this != &other)
        {
            m_Name = std::move(other.m_Name);
            m_Address = other.m_Address;
            other.m_Address = nullptr; // Ensure the moved-from object has a null address
        }
        return *this;
    }

    BLACKBASE_API const std::string& Export::GetName() const
    {
        return m_Name;
    }

    BLACKBASE_API void* Export::GetAddress() const
    {
        return m_Address;
    }
}