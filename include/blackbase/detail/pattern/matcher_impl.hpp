#pragma once
#include <blackbase/pattern/matcher.hpp>
#include <blackbase/library/library.hpp>
#include <blackbase/internal/assert.hpp>


#ifdef BLACKBASE_SYSTEM_WINDOWS
    #include <blackbase/internal/defs/windows.hpp>

    inline std::vector<uint8_t> ReadBlock(size_t Begin, size_t End)
    {
        std::vector<uint8_t> block;
        auto BlockSize = End - Begin;

        block.reserve(BlockSize);

        uintptr_t address = Begin;
        
        while (address < End && block.size() < BlockSize)
        {
            MEMORY_BASIC_INFORMATION mbi{};
            if (VirtualQuery(reinterpret_cast<PVOID>(address), &mbi, sizeof(mbi)) == 0)
            {
                BLACKBASE_ERROR("VirtualQuery failed at address: {}", address);
                break;
            }

            uintptr_t region_start = std::max(address, reinterpret_cast<uintptr_t>(mbi.BaseAddress));
            uintptr_t region_end = std::min(End, region_start + mbi.RegionSize);
            size_t to_read = std::min(BlockSize - block.size(), region_end - region_start);

            if (mbi.Protect & PAGE_GUARD || mbi.Protect == PAGE_NOACCESS)
            {
                BLACKBASE_ERROR("Memory at address {} is not accessible", region_start);
                address = region_end; // Skip to the next region
                
                continue;
            }

            bool isReadable = (mbi.Protect == PAGE_READONLY) || 
                              (mbi.Protect == PAGE_READWRITE) ||
                              (mbi.Protect == PAGE_EXECUTE_READ) ||
                              (mbi.Protect == PAGE_EXECUTE_READWRITE);

            if (isReadable)
            {
                const uint8_t* src = reinterpret_cast<const uint8_t*>(region_start);
                block.insert(block.end(), src, src + to_read);
                address += mbi.RegionSize;
            }
        }
        

        return block;
    }
#else
    inline std::vector<uint8_t> ReadBlock(size_t Begin, size_t End)
    {
        std::vector<uint8_t> block;
        block.reserve(End - Begin);

        static_assert(Begin <= End, "Begin must be less than End");

        std::memcpy(block.data(), reinterpret_cast<const uint8_t*>(Begin), End - Begin);

        return block;
    }
#endif


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
        auto library = blackbase::library::Library::GetLibraryHandle(moduleName);
        
        BLACKBASE_ASSERT(library.has_value(), "Module '{}' not found", moduleName);

        m_moduleBase = library->GetHandle();
        m_moduleSize = library->GetSize();
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR void Matcher::setModule(const std::uintptr_t moduleBase, const std::size_t moduleSize)
    {
        m_moduleBase = moduleBase;
        m_moduleSize = moduleSize;
    }

    BLACKBASE_API std::vector<Match> Matcher::findAll(const Pattern& pattern) const
    {
        std::vector<Match> matches;
        std::vector<uint8_t> moduleData = ReadBlock(m_moduleBase, m_moduleBase + m_moduleSize);
    
        for (size_t i = 0; i <= moduleData.size() - pattern.getBytes().size(); ++i)
        {
            bool match = true;

            for (size_t j = 0; j < pattern.getBytes().size(); ++j)
            {
                if (pattern.getMask()[j] && pattern.getBytes()[j] != moduleData[i + j])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                matches.emplace_back(m_moduleBase + i);
            }
        }

        return matches;
    }

    BLACKBASE_API std::optional<Match> Matcher::findFirst(const Pattern& pattern) const
    {
        std::vector<Match> matches;
        std::vector<uint8_t> moduleData = ReadBlock(m_moduleBase, m_moduleBase + m_moduleSize);
    
        for (size_t i = 0; i <= moduleData.size() - pattern.getBytes().size(); ++i)
        {
            bool match = true;

            for (size_t j = 0; j < pattern.getBytes().size(); ++j)
            {
                if (pattern.getMask()[j] && pattern.getBytes()[j] != moduleData[i + j])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                return Match(m_moduleBase + i);
            }
        }

        return std::nullopt;
    }   
}