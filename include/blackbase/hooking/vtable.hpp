#pragma once
#include <blackbase/common.hpp>
#include <blackbase/hooking/hook.hpp>

namespace blackbase::hooking
{
    class VTableHook : public Hook
    {
    private:
        void* m_OriginalFunction;
        void* m_HookedFunction;
        void* m_Object;
        std::size_t m_HookedIndex;
        
    protected:
        VTableHook(void* object, std::size_t index, void* newFunction);

    public:
        BLACKBASE_API static std::shared_ptr<VTableHook> Create(void* object, std::size_t index, void* newFunction);

    public:
        BLACKBASE_API virtual ~VTableHook();

        BLACKBASE_API virtual void* GetOriginalFunction() const override;
        BLACKBASE_API virtual void* GetHookedFunction() const override;

        BLACKBASE_API virtual const std::string_view& GetType() const override;
        
        BLACKBASE_API virtual bool IsEnabled() const override;
        BLACKBASE_API virtual void Enable() override;
        BLACKBASE_API virtual void Disable() override;

        BLACKBASE_API virtual bool IsValid() const override;
    };
}