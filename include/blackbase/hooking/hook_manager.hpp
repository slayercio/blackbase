#pragma once
#include <blackbase/common.hpp>
#include <blackbase/hooking/hook.hpp>

namespace blackbase::hooking
{
    class HookManager
    {
    private:
        std::vector<std::shared_ptr<Hook>> m_Hooks;

    public:
        BLACKBASE_API HookManager(); 

    public:
        BLACKBASE_API void RegisterHook(std::shared_ptr<Hook> hook);
        BLACKBASE_API void UnregisterHook(std::size_t index);

        BLACKBASE_API std::optional<std::shared_ptr<Hook>> FindHook(std::function<bool(const std::shared_ptr<Hook>&)> predicate) const;
        BLACKBASE_API std::optional<std::shared_ptr<Hook>> GetHook(std::size_t index) const;
        BLACKBASE_API const std::vector<std::shared_ptr<Hook>>& GetAllHooks() const;

        BLACKBASE_API void EnableAllHooks();
        BLACKBASE_API void DisableAllHooks();
    public:
        BLACKBASE_API ~HookManager();
        
        // non-copyable
        BLACKBASE_API HookManager(const HookManager&) = delete;
        BLACKBASE_API HookManager& operator=(const HookManager&) = delete;
        
        // movable
        BLACKBASE_API HookManager(HookManager&&);
        BLACKBASE_API HookManager& operator=(HookManager&&);
    };
}