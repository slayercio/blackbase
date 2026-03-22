#pragma once
#include <blackbase/common.hpp>
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/pattern/match.hpp>
#include <blackbase/library/library.hpp>

namespace blackbase
{
    class Matcher
    {
    private:
        std::uintptr_t m_ModuleBase;
        std::uintptr_t m_ModuleEnd;

    public:
        inline Matcher() noexcept = default;
        inline Matcher(std::uintptr_t moduleBase, std::uintptr_t moduleEnd) noexcept
        {
            m_ModuleBase = moduleBase;
            m_ModuleEnd  = moduleEnd;
        }

        inline Matcher(const std::string_view& moduleName) noexcept;

    public:
        inline std::vector<Match>   FindAll(const Pattern& pattern) const noexcept;
        inline std::optional<Match> FindFirst(const Pattern& pattern) const noexcept;
    };
}

#pragma region Implementation
namespace blackbase
{
    #ifdef BLACKBASE_IMPL_AVX2
        #include <immintrin.h>
        inline bool MatchAVX2(
            const std::uint8_t* data,
            const std::uint8_t* patternBytes,
            const std::uint8_t* patternMask,
            std::size_t size
        )
        {
            std::size_t i = 0;

            for(; i + 32 <= size; i += 32)
            {
                __m256i d = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
                __m256i p = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(patternBytes + i));
                __m256i m = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(patternMask + i));

                __m256i diff = _mm256_xor_si256(d, p);
                __m256i masked = _mm256_and_si256(diff, m);

                if (!_mm256_testz_si256(masked, masked))
                {
                    return false;
                }
            }

            for (; i + 16 <= size; i += 16)
            {
                __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
                __m128i p = _mm_loadu_si128(reinterpret_cast<const __m128i*>(patternBytes + i));
                __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i*>(patternMask + i));
                
                __m128i diff = _mm_xor_si128(d, p);
                __m128i masked = _mm_and_si128(diff, m);

                if (!_mm_testz_si128(masked, masked))
                {
                    return false;
                }
            }

            for (; i < size; i++)
            {
                if (patternMask[i] && patternBytes[i] != data[i])
                {
                    return false;
                }
            }

            return true;
        }
    #endif

    #ifdef BLACKBASE_IMPL_SSE2
        #include <emmintrin.h>

        inline bool ScanSSE2(
            const std::uint8_t* data,
            const std::uint8_t* patternBytes,
            const std::uint8_t* patternMask,
            std::size_t size
        )
        {
            std::size_t i = 0;

            for (; i + 16 <= size; i += 16)
            {
                __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
                __m128i p = _mm_loadu_si128(reinterpret_cast<const __m128i*>(patternBytes + i));
                __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i*>(patternMask + i));
                
                __m128i diff = _mm_xor_si128(d, p);
                __m128i masked = _mm_and_si128(diff, m);

                if (!_mm_testz_si128(masked, masked))
                {
                    return false;
                }
            }

            for (; i < size; i++)
            {
                if (patternMask[i] && patternBytes[i] != data[i])
                {
                    return false;
                }
            }

            return true;
        }
    #endif

    inline bool ScanRaw(
        const std::uint8_t* data,
        const std::uint8_t* patternBytes,
        const std::uint8_t* patternMask,
        std::size_t size
    )
    {
        for (std::size_t i = 0; i < size; i++)
        {
            if (patternMask[i] && patternBytes[i] != data[i])
            {
                return false;
            }
        }

        return true;
    }

    #include <intrin.h>
    inline bool HasSSE2Support()
    {
        int info[4];
        __cpuid(info, 1);

        return (info[3] & (1 << 26)) != 0;
    }

    inline bool HasAVX2Support()
    {
        int info[4];
        __cpuidex(info, 7, 0);

        return (info[1] & (1 << 5)) != 0;
    }

    enum ScanMode
    {
        AVX2,
        SSE2,
        Raw
    };

    inline ScanMode GetBestScanMode()
    {
        static ScanMode mode = []() {
            if (HasAVX2Support())
            {
                return ScanMode::AVX2;
            }
            else if (HasSSE2Support())
            {
                return ScanMode::SSE2;
            }
            else
            {
                return ScanMode::Raw;
            }
        }();

        return mode;
    }

    inline bool MatchPattern(
        const std::uint8_t* data,
        const std::uint8_t* patternBytes,
        const std::uint8_t* patternMask,
        std::size_t size
    )
    {
        if (size == 0 || (patternMask[0] && data[0] != patternBytes[0]))
        {
            // Quick check to avoid unnecessary SIMD processing if the first byte doesn't match.

            return false;
        }

        switch (GetBestScanMode())
        {
        #ifdef BLACKBASE_IMPL_AVX2
            case ScanMode::AVX2:
                return MatchAVX2(data, patternBytes, patternMask, size);
        #endif

        #ifdef BLACKBASE_IMPL_SSE2
            case ScanMode::SSE2:
                return ScanSSE2(data, patternBytes, patternMask, size);
        #endif

            default:
                return ScanRaw(data, patternBytes, patternMask, size);
        }
    }

    inline Matcher::Matcher(const std::string_view& moduleName) noexcept
        : m_ModuleBase(0), m_ModuleEnd(0)
    {
        const auto library = Library::FindByName(moduleName);
        if (library.has_value())
        {
            m_ModuleBase = library->GetBaseAddress();
            m_ModuleEnd  = library->GetEndAddress();
        }
        else
        {
            runtime_throw<std::runtime_error>(xorstr_("Module not found: {}"), moduleName);
        }
    }

    inline std::vector<Match> Matcher::FindAll(const Pattern& pattern) const noexcept
    {
        if (m_ModuleBase == 0 || m_ModuleEnd == 0)
        {
            return {};
        }

        std::vector<Match> matches;
        size_t currentAddress = m_ModuleBase;
        size_t endAddress = m_ModuleEnd;

        MEMORY_BASIC_INFORMATION mbi;

        size_t patternSize = pattern.GetBytes().size();
        size_t pageSize = blackbase::windows::GetPageSize();

        auto patternBytes = pattern.GetBytes();
        auto patternMask = pattern.GetMask();

        while (currentAddress + patternSize < endAddress)
        {
            if (!VirtualQuery(reinterpret_cast<LPCVOID>(currentAddress), &mbi, sizeof(mbi)))
            {
                currentAddress += pageSize;
                continue;
            }

            size_t regionEnd = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
            if (regionEnd > endAddress)
            {
                regionEnd = endAddress;
            }

            if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_NOACCESS) && !(mbi.Protect & PAGE_GUARD))
            {
                size_t bytesToCheck = regionEnd - currentAddress;

                const uint8_t* address = reinterpret_cast<const uint8_t*>(currentAddress);

                for (size_t i = 0; i <= bytesToCheck - patternSize; i++)
                {
                    if (!MatchPattern(address + i, patternBytes.data(), patternMask.data(), patternSize))
                    {
                        continue;
                    }

                    matches.emplace_back(currentAddress + i);
                }
            }

            currentAddress = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
            if (currentAddress <= reinterpret_cast<size_t>(mbi.BaseAddress))
            {
                break; // Prevent infinite loop if the address does not progress
            }
        }

        return matches;
    }

    inline std::optional<Match> Matcher::FindFirst(const Pattern& pattern) const noexcept
    {
        if (m_ModuleBase == 0 || m_ModuleEnd == 0)
        {
            return std::nullopt;
        }

        size_t currentAddress = m_ModuleBase;
        size_t endAddress = m_ModuleEnd;

        MEMORY_BASIC_INFORMATION mbi;

        size_t patternSize = pattern.GetBytes().size();
        size_t pageSize = blackbase::windows::GetPageSize();

        auto patternBytes = pattern.GetBytes();
        auto patternMask = pattern.GetMask();

        while (currentAddress + patternSize < endAddress)
        {
            if (!VirtualQuery(reinterpret_cast<LPCVOID>(currentAddress), &mbi, sizeof(mbi)))
            {
                currentAddress += pageSize;
                continue;
            }

            size_t regionEnd = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
            if (regionEnd > endAddress)
            {
                regionEnd = endAddress;
            }

            if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_NOACCESS) && !(mbi.Protect & PAGE_GUARD))
            {
                size_t bytesToCheck = regionEnd - currentAddress;

                const uint8_t* address = reinterpret_cast<const uint8_t*>(currentAddress);

                for (size_t i = 0; i <= bytesToCheck - patternSize; i++)
                {
                    if (!MatchPattern(address + i, patternBytes.data(), patternMask.data(), patternSize))
                    {
                        continue;
                    }

                    return Match{ currentAddress + i };
                }
            }

            currentAddress = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
            if (currentAddress <= reinterpret_cast<size_t>(mbi.BaseAddress))
            {
                break; // Prevent infinite loop if the address does not progress
            }
        }

        return std::nullopt;
    }


}
#pragma endregion