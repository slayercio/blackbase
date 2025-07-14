#pragma once
#include <blackbase/common.hpp>
#include <blackbase/library/export.hpp>

namespace blackbase::library
{
    class Library
    {
    private:
        std::uintptr_t m_Handle;
        std::size_t    m_Size;
        std::string    m_Name;

    public:
        static BLACKBASE_API std::optional<Library> GetLibraryHandle(const std::string_view& name);
        static BLACKBASE_API std::optional<Library> GetLibraryHandle(std::uintptr_t address);

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Library();
        BLACKBASE_API BLACKBASE_CONSTEXPR Library(std::uintptr_t handle, std::size_t size, const std::string_view& name);

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Library(const Library& other);
        BLACKBASE_API BLACKBASE_CONSTEXPR Library& operator=(const Library& other);

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Library(Library&& other) noexcept;
        BLACKBASE_API BLACKBASE_CONSTEXPR Library& operator=(Library&& other) noexcept;

    public:
        BLACKBASE_API std::optional<blackbase::library::Export> GetExport(const std::string_view& name) const;
        BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t GetHandle() const;
        BLACKBASE_API BLACKBASE_CONSTEXPR const std::string& GetName() const;
        BLACKBASE_API BLACKBASE_CONSTEXPR std::size_t GetSize() const;
        BLACKBASE_API BLACKBASE_CONSTEXPR bool IsValid() const;
    };
}