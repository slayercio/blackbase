#pragma once
#include <blackbase/common.hpp>
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/pattern/match.hpp>

namespace blackbase::pattern
{
    class Matcher
    {
    private:
        std::uintptr_t m_moduleBase; // Base address of the module
        std::size_t    m_moduleSize; // Size of the module
        
    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Matcher();
        BLACKBASE_API BLACKBASE_CONSTEXPR Matcher(const std::uintptr_t moduleBase, const std::size_t moduleSize);
        BLACKBASE_API Matcher(const std::string_view& moduleName);

    public:
        BLACKBASE_API void setModule(const std::string_view& moduleName);
        BLACKBASE_API BLACKBASE_CONSTEXPR void setModule(const std::uintptr_t moduleBase, const std::size_t moduleSize);

    public:
        BLACKBASE_API std::vector<Match> findAll(const Pattern& pattern) const;
        BLACKBASE_API std::optional<Match> findFirst(const Pattern& pattern) const;
    };
}

#ifdef BLACKBASE_HEADER_ONLY
#include <blackbase/detail/pattern/matcher_impl.hpp>
#endif