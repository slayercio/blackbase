#pragma once
#include <blackbase/hooking/vtable.hpp>

namespace blackbase::hooking
{
    VTableHook::VTableHook(void* object, std::size_t index, void* newFunction)
        : Hook(), m_OriginalFunction(nullptr), m_HookedFunction(newFunction), m_HookedIndex(index), m_Object(object)
    {
    }

    std::shared_ptr<VTableHook> VTableHook::Create(void* object, std::size_t index, void* newFunction)
    {
        return std::make_shared<VTableHook>(object, index, newFunction);
    }

    VTableHook::~VTableHook() = default;

    void* VTableHook::GetOriginalFunction() const
    {
        return m_OriginalFunction;
    }

    void* VTableHook::GetHookedFunction() const
    {
        return m_HookedFunction;
    }

    const std::string_view& VTableHook::GetType() const
    {
        static const std::string_view type = "VTableHook";
        return type;
    }

    bool VTableHook::IsEnabled() const
    {
        
    }

    void VTableHook::Enable()
    {
        void **vtable = *reinterpret_cast<void***>(m_Object);
        m_OriginalFunction = vtable[m_HookedIndex];
        vtable[m_HookedIndex] = m_HookedFunction; // Hook the function
    }

    void VTableHook::Disable()
    {
        void **vtable = *reinterpret_cast<void***>(m_Object);
        vtable[m_HookedIndex] = m_OriginalFunction; // Restore the original function
        m_OriginalFunction = nullptr; // Clear the original function pointer
    }

    bool VTableHook::IsValid() const
    {
        if (m_Object == nullptr || m_HookedIndex < 0)
        {
            return false;
        }

        if (m_HookedFunction == nullptr)
        {
            return false;
        }

        if (m_OriginalFunction == nullptr)
        {
            return false;
        }

        void **vtable = *reinterpret_cast<void***>(m_Object);

        if (vtable == nullptr || vtable[m_HookedIndex] != m_HookedFunction)
        {
            return false;
        }

        return true;
    }
}