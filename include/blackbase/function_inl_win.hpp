#pragma once
#include <windows.h>
#include <vector>
#include <mutex>
#include <utility>
#include <functional>
#include <memory>
#include <set>

#include <blackbase/functional.hpp>

namespace blackbase::functional
{
    namespace detail
    {
        struct FunctionWrapData
        {
            void* this_ptr;
            std::size_t size;
            ThunkAllocator::deleter_t deleter;
            std::function<void(void*, std::size_t)> deallocator;
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
                    }

                    if (data.stub_ptr && data.deallocator)
                    {
                        data.deallocator(data.stub_ptr, data.size);
                    }
                }
            }
        };

        static FunctionWrapStorage s_WrapStorage;

        struct PagedAllocatorStorage
        {   
            static constexpr std::size_t DEFAULT_PAGE_SIZE = 128 * 1024; // 128KB

            static inline std::size_t align_up(std::size_t value, std::size_t alignment)
            {
                return (value + alignment - 1) & ~(alignment - 1);
            }

            struct Page
            {
                std::uintptr_t base;
                std::size_t size;
                std::mutex mutex;
                
                std::size_t cursor = 0;

                Page(std::uintptr_t base, std::size_t size) : base(base), size(size)
                {
                }
            };

            std::vector<std::shared_ptr<Page>> pages{};
            std::mutex mutex;

            std::shared_ptr<Page> add_page(std::size_t size)
            {
                void* mem = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                if (!mem)
                {
                    throw std::bad_alloc();
                }

                return pages.emplace_back(std::make_shared<Page>(reinterpret_cast<std::uintptr_t>(mem), size));
            }

            std::set<void*> already_allocated; // debug

            std::pair<void*, std::size_t> allocate(std::size_t size, std::size_t alignment)
            {
                std::lock_guard lock(mutex);
                for (auto& page : pages)
                {
                    std::lock_guard page_lock(page->mutex);
                    std::size_t aligned = align_up(page->cursor, alignment);
                    if (aligned + size <= page->size)
                    {
                        void* ptr = reinterpret_cast<void*>(page->base + aligned);
                        page->cursor = aligned + size;
                        return { ptr, size };
                    }
                }

                std::size_t page_size = align_up(size, DEFAULT_PAGE_SIZE);
                std::shared_ptr<Page> new_page = add_page(page_size);

                std::size_t aligned = align_up(new_page->cursor, alignment);
                new_page->cursor = aligned + size;

                return { reinterpret_cast<void*>(new_page->base + aligned), size };
            }

            ~PagedAllocatorStorage()
            {
                for (auto& page : pages)
                {
                    VirtualFree(reinterpret_cast<void*>(page->base), 0, MEM_RELEASE);
                }
            }
        };

        static PagedAllocatorStorage s_PagedStorage;
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
            data.deallocator = [](void* ptr, std::size_t size)
            {
                ThunkAllocator::deallocate(ptr, size);
            };
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

    void* PagedThunkAllocator::allocate(std::size_t size, std::size_t alignment)
    {
        auto [ptr, alloc_size] = detail::s_PagedStorage.allocate(size, alignment);
        auto it = detail::s_PagedStorage.already_allocated.find(ptr);
        if (it != detail::s_PagedStorage.already_allocated.end())
        {
            __debugbreak(); // double allocation of the same pointer, something went wrong
        }

        detail::s_PagedStorage.already_allocated.insert(ptr);
        
        return ptr;
    }

    void PagedThunkAllocator::deallocate(void* stub_ptr, std::size_t size)
    {
        // Deallocation is handled by the PagedAllocatorStorage destructor
    }

    void PagedThunkAllocator::track(void* this_ptr, void* stub_ptr, std::size_t size, bool auto_delete, deleter_t deleter)
    {
        if (stub_ptr)
        {
            detail::FunctionWrapData data;
            data.this_ptr = this_ptr;
            data.size = size;
            data.deleter = deleter;
            data.stub_ptr = stub_ptr;
            data.auto_delete = auto_delete;
            data.deallocator = [](void* ptr, std::size_t size)
            {
                // Deallocation is handled by the PagedAllocatorStorage destructor
            };
            detail::s_WrapStorage.add_wrap(data);
        }
    }

    void PagedThunkAllocator::finish(void* ptr, std::size_t size)
    {
        // we need to keep the pages RWX to allow for dynamic code generation, so we won't change the protection here
    }
}