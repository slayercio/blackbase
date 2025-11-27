#pragma once
#include <blackbase/common.hpp>
#include <Windows.h>
#include <winternl.h>

namespace blackbase::windows
{
    typedef struct _LDR_DATA_TABLE_ENTRY {
        LIST_ENTRY InLoadOrderLinks;
        LIST_ENTRY InMemoryOrderLinks;
        LIST_ENTRY InInitializationOrderLinks;
        PVOID DllBase;
        PVOID EntryPoint;
        ULONG SizeOfImage;
        UNICODE_STRING FullDllName;
        UNICODE_STRING BaseDllName;
        ULONG Flags;
        WORD LoadCount;
        WORD TlsIndex;
        union {
            LIST_ENTRY HashLinks;
            struct {
                PVOID SectionPointer;
                ULONG CheckSum;
            };
        };
        union {
            ULONG TimeDateStamp;
            PVOID LoadedImports;
        };
        struct _ACTIVATION_CONTEXT *EntryPointActivationContext;
        PVOID PatchInformation;
        LIST_ENTRY ForwarderLinks;
        LIST_ENTRY ServiceTagLinks;
        LIST_ENTRY StaticLinks;
    } LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

    inline std::size_t GetPageSize()
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwPageSize;
    }
}