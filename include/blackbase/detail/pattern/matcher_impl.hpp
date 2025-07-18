#pragma once
#include <blackbase/pattern/matcher.hpp>
#include <blackbase/library/library.hpp>
#include <blackbase/internal/assert.hpp>
#include <blackbase/internal/system.hpp>

namespace blackbase::pattern
{
    BLACKBASE_API BLACKBASE_CONSTEXPR Matcher::Matcher()
        : Matcher(0, 0)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Matcher::Matcher(const std::uintptr_t moduleBase, const std::size_t moduleSize)
        : m_moduleBase(moduleBase), m_moduleSize(moduleSize)
    {
    }

    BLACKBASE_API Matcher::Matcher(const std::string_view& moduleName)
    {
        setModule(moduleName);
    }

    BLACKBASE_API void Matcher::setModule(const std::string_view& moduleName)
    {
        auto library = ::blackbase::library::Library::GetLibraryHandle(moduleName);
        
        BLACKBASE_ASSERT(library.has_value(), "Module '{}' not found", moduleName);

        m_moduleBase = library->GetHandle();
        m_moduleSize = library->GetSize();
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR void Matcher::setModule(const std::uintptr_t moduleBase, const std::size_t moduleSize)
    {
        m_moduleBase = moduleBase;
        m_moduleSize = moduleSize;
    }

    
    
}

#ifdef BLACKBASE_SYSTEM_WINDOWS
    #include <blackbase/detail/pattern/matcher_impl_safe_win.hpp>
#else
    BLACKBASE_API std::vector<Match> Matcher::findAll(const Pattern& pattern) const
    {
        std::vector<Match> matches;

        for (uint8_t* address = reinterpret_cast<uint8_t*>(m_moduleBase); address + pattern.getBytes().size() < reinterpret_cast<uint8_t*>(m_moduleBase + m_moduleSize); ++address)
        {
            bool match = true;

            for (size_t i = 0; i < pattern.getBytes().size(); ++i)
            {
                if (pattern.getMask()[i] && pattern.getBytes()[i] != address[i])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                matches.emplace_back(reinterpret_cast<std::uintptr_t>(address));
            }
        }

        return matches;
    }

    BLACKBASE_API std::optional<Match> Matcher::findFirst(const Pattern& pattern) const
    {
        for (uint8_t* address = reinterpret_cast<uint8_t*>(m_moduleBase); address + pattern.getBytes().size() < reinterpret_cast<uint8_t*>(m_moduleBase + m_moduleSize); ++address)
        {
            bool match = true;

            for (size_t i = 0; i < pattern.getBytes().size(); ++i)
            {
                if (pattern.getMask()[i] && pattern.getBytes()[i] != address[i])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                return Match(reinterpret_cast<std::uintptr_t>(address));
            }
        }

        return std::nullopt;
    }
#endif