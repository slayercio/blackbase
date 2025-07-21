#pragma once
#include <blackbase/hooking/hook_manager.hpp>

namespace blackbase::hooking
{
    BLACKBASE_API HookManager::HookManager()
    {
        m_Hooks.reserve(10);
    }

    BLACKBASE_API void HookManager::RegisterHook(std::shared_ptr<Hook> hook)
    {
        if (hook)
        {
            m_Hooks.push_back(std::move(hook));
        }
    }

    BLACKBASE_API void HookManager::UnregisterHook(std::size_t index)
    {
        if (index < m_Hooks.size())
        {
            m_Hooks.erase(m_Hooks.begin() + index);
        }
    }

    BLACKBASE_API std::optional<std::shared_ptr<Hook>> HookManager::FindHook(std::function<bool(const std::shared_ptr<Hook>&)> predicate) const
    {
        for (const auto& hook : m_Hooks)
        {
            if (predicate(hook))
            {
                return hook;
            }
        }

        return std::nullopt;
    }

    BLACKBASE_API std::optional<std::shared_ptr<Hook>> HookManager::GetHook(std::size_t index) const
    {
        if (index < m_Hooks.size())
        {
            return m_Hooks[index];
        }

        return std::nullopt;
    }

    BLACKBASE_API const std::vector<std::shared_ptr<Hook>>& HookManager::GetAllHooks() const
    {
        return m_Hooks;
    }

    BLACKBASE_API void HookManager::EnableAllHooks()
    {
        for (const auto& hook : m_Hooks)
        {
            if (hook && !hook->IsEnabled())
            {
                hook->Enable();
            }
        }
    }

    BLACKBASE_API void HookManager::DisableAllHooks()
    {
        for (const auto& hook : m_Hooks)
        {
            if (hook && hook->IsEnabled())
            {
                hook->Disable();
            }
        }
    }

    BLACKBASE_API HookManager::~HookManager()
    {
        for (const auto& hook : m_Hooks)
        {
            if (hook && hook->IsEnabled())
            {
                hook->Disable();
            }
        }

        m_Hooks.clear();
    }

    BLACKBASE_API HookManager::HookManager(HookManager&& other)
        : m_Hooks(std::move(other.m_Hooks))
    {
        other.m_Hooks.clear();
    }

    BLACKBASE_API HookManager& HookManager::operator=(HookManager&& other)
    {
        if (this != &other)
        {
            m_Hooks = std::move(other.m_Hooks);
            other.m_Hooks.clear();
        }
     
        return *this;
    }
}