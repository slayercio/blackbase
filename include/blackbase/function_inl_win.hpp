#pragma once
#include <windows.h>
#include <vector>
#include <mutex>

#include <blackbase/functional.hpp>

#pragma section(".blackbase_func", read, execute)
__declspec(allocate(".blackbase_func")) unsigned char blackbase_read_r10_stub[] =
{
    0x4C, 0x89, 0xD0,       // mov rax, r10
    0xC3                    // ret
};

namespace blackbase::functional
{
    namespace detail
    {
        struct FunctionWrapData
        {
            void* this_ptr;
            std::size_t size;
            ThunkAllocator::deleter_t deleter;
            void* stub_ptr;
            bool auto_delete;       
        };

        struct FunctionWrapStorage
        {
            std::vector<FunctionWrapData> wraps;
            std::mutex mutex;

            void add_wrap(const FunctionWrapData& data)
            {
                std::lock_guard lock(mutex);
                wraps.push_back(data);
            }

            ~FunctionWrapStorage()
            {
                for (const auto& data : wraps)
                {
                    if (data.auto_delete && data.deleter)
                    {
                        data.deleter(data.this_ptr);
                        ThunkAllocator::deallocate(data.stub_ptr, data.size);
                    }
                }
            }
        };

        static FunctionWrapStorage s_WrapStorage;
    }

    void* ThunkAllocator::allocate(std::size_t size, std::size_t alignment)
    {
        return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }

    void ThunkAllocator::deallocate(void* ptr, std::size_t size)
    {
        if (ptr)
        {
            VirtualFree(ptr, 0, MEM_RELEASE);
        }
    }

    void ThunkAllocator::track(void* this_ptr, void* stub_ptr, std::size_t size, bool auto_delete, deleter_t deleter)
    {
        if (stub_ptr)
        {
            detail::FunctionWrapData data;
            data.this_ptr = this_ptr;
            data.size = size;
            data.deleter = deleter;
            data.stub_ptr = stub_ptr;
            data.auto_delete = auto_delete;
            detail::s_WrapStorage.add_wrap(data);
        }
    }
    
    void ThunkAllocator::finish(void* ptr, std::size_t size)
    {
        if (ptr)
        {
            DWORD oldProtect;
            VirtualProtect(ptr, size, PAGE_EXECUTE_READ, &oldProtect);
        }
    }
}