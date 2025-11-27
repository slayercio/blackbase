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
            void(*deleter)(void*);
            void* stub_ptr;
        };

        struct FunctionWrapStorage
        {
            std::vector<FunctionWrapData> data_storage;
            std::mutex storage_mutex;

            std::size_t add(FunctionWrapData&& data)
            {
                std::lock_guard lock(storage_mutex);
                data_storage.push_back(std::move(data));
                return data_storage.size() - 1;
            }

            ~FunctionWrapStorage()
            {
                std::lock_guard lock(storage_mutex);

                for (auto& data : data_storage)
                {
                    if (data.deleter)
                    {
                        data.deleter(data.this_ptr);
                    }

                    if (data.stub_ptr)
                    {
                        VirtualFree(data.stub_ptr, 0, MEM_RELEASE);
                    }
                }

                data_storage.clear();
            }
        };

        static FunctionWrapStorage s_WrapStorage;
    }

    void* FunctionWrapper::create_thunk(void* this_ptr, void* dispatch_ptr, deleter_t deleter)
    {
        constexpr std::size_t thunk_size = 32;

        void* stub = VirtualAlloc(nullptr, thunk_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!stub)
        {
            return nullptr;
        }

        unsigned char* code = reinterpret_cast<unsigned char*>(stub);
        // mov r10, this_ptr
        *code++ = 0x49; *code++ = 0xBA;
        *reinterpret_cast<void**>(code) = this_ptr;
        code += sizeof(void*);

        // mov rax, dispatch_ptr
        *code++ = 0x48; *code++ = 0xB8;
        *reinterpret_cast<void**>(code) = dispatch_ptr;
        code += sizeof(void*);

        // jmp rax
        *code++ = 0xFF; *code++ = 0xE0;
    
        detail::FunctionWrapData data;
        data.this_ptr = this_ptr;
        data.deleter = deleter;
        data.stub_ptr = stub;
        detail::s_WrapStorage.add(std::move(data));

        return stub;
    }

    void* FunctionWrapper::read_r10()
    {
        using read_r10_t = void* (*)();
        auto func = reinterpret_cast<read_r10_t>(reinterpret_cast<unsigned char*>(blackbase_read_r10_stub));
        return func();
    }
}