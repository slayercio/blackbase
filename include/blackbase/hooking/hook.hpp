#pragma once
#include <blackbase/common.hpp>

namespace blackbase::hooking
{
    class Hook
    {
    public:
        virtual ~Hook() = default;

        virtual void* GetOriginalFunction() const = 0;
        virtual void* GetHookedFunction() const = 0;

        virtual const std::string_view& GetType() const = 0;
        virtual bool IsEnabled() const = 0;

        virtual void Enable() = 0;
        virtual void Disable() = 0;

        virtual bool IsValid() const = 0;
    };
}