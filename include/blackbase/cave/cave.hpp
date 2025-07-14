#pragma once
#include <blackbase/common.hpp>

namespace blackbase
{
    enum class SectionFlags : std::uint32_t
    {
        None = 0,
        Readable = 1 << 0,
        Writable = 1 << 1,
        Executable = 1 << 2
    };

    class Cave
    {
    private:
        std::uintptr_t          m_Address;
        blackbase::SectionFlags m_Flags;
    
    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Cave();
        BLACKBASE_API BLACKBASE_CONSTEXPR Cave(std::uintptr_t address, blackbase::SectionFlags flags);

        BLACKBASE_API BLACKBASE_CONSTEXPR Cave(const Cave& other);
        BLACKBASE_API BLACKBASE_CONSTEXPR Cave(Cave&& other) noexcept;
        BLACKBASE_API Cave& operator=(const Cave& other);
        BLACKBASE_API Cave& operator=(Cave&& other) noexcept;

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t GetAddress() const;
        BLACKBASE_API BLACKBASE_CONSTEXPR blackbase::SectionFlags GetFlags() const;

        BLACKBASE_API BLACKBASE_CONSTEXPR bool IsReadable() const;
        BLACKBASE_API BLACKBASE_CONSTEXPR bool IsWritable() const;
        BLACKBASE_API BLACKBASE_CONSTEXPR bool IsExecutable() const;

        BLACKBASE_API BLACKBASE_CONSTEXPR void SetAddress(std::uintptr_t address);
        BLACKBASE_API BLACKBASE_CONSTEXPR void SetFlags(blackbase::SectionFlags flags);

        BLACKBASE_API BLACKBASE_CONSTEXPR blackbase::SectionFlags AddFlag(blackbase::SectionFlags flag);
        BLACKBASE_API BLACKBASE_CONSTEXPR blackbase::SectionFlags RemoveFlag(blackbase::SectionFlags flag);
    };
}