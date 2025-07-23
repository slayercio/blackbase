#pragma once
#include <blackbase/common.hpp>
#include <blackbase/cave/cave.hpp>

namespace blackbase
{
    class ICaveFinder
    {
    public:
        BLACKBASE_API virtual ~ICaveFinder() = default;

        BLACKBASE_API virtual std::optional<Cave> findCave(const std::string& moduleName, size_t requiredSize) const = 0;
        BLACKBASE_API virtual std::vector<Cave> findCaves(const std::string& moduleName, size_t requiredSize) const = 0;
    };

    BLACKBASE_API ICaveFinder* CreateCaveFinder();
}