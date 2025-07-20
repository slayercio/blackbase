#include <Windows.h>
#include <DbgHelp.h>

#include <blackbase/blackbase.hpp>
#include <blackbase/caller/indirect_call.hpp>

#include <intrin.h>
#include <print>
#include <ranges>



#pragma comment(lib, "Dbghelp.lib")

void PrintStacktrace()
{
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    CONTEXT context = {};
    RtlCaptureContext(&context);
    
    SymInitialize(hProcess, nullptr, TRUE);
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

    STACKFRAME64 stackFrame = {};
    DWORD imageType = IMAGE_FILE_MACHINE_AMD64;

    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    for (int i = 0; i < 64; i++)
    {
        if (!StackWalk64(imageType, hProcess, hThread, &stackFrame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
        {
            break;
        }

        DWORD64 address = stackFrame.AddrPC.Offset;
        if (address == 0)
        {
            break;
        }

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
        PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        DWORD64 displacement = 0;
        if (SymFromAddr(hProcess, address, &displacement, symbol))
        {
            std::println("Function: {} at address: 0x{:X}", symbol->Name, symbol->Address);
        }
        else
        {
            std::println("Failed to retrieve symbol for address: 0x{:X}", address);
        }
    }

    SymCleanup(hProcess);
}

#pragma section(".text")
__declspec(allocate(".text")) const unsigned char shellcode[] = { 0xFF, 0x23 }; // jmp rbx

int main();

int function(int a, int b, int c, int d, int e)
{
    const auto ret = _ReturnAddress();
    std::println("Ret == main: {}, Ret == shellcode: {}", ret == (void*)main, ret == (void*)shellcode);
    std::println("a: {}, b: {}, c: {}, d: {}, e: {}", a, b, c, d, e);

    PrintStacktrace();

    return a + b + c + d + e;
}

int main()
{
    auto shellcodePattern = blackbase::pattern::Pattern("FF 23");
    auto matches = blackbase::pattern::Matcher("kernel32.dll").findAll(shellcodePattern);
    auto executableMatches = blackbase::pattern::Match::filterExecutable(matches);

    void *call_stub = const_cast<void*>(reinterpret_cast<const void*>(shellcode));

    if (!executableMatches.empty())
    {
        std::println("Found {} executable matches for the shellcode pattern.", executableMatches.size());
        std::println("First match address: 0x{:X}", executableMatches.front().getAddress());

        call_stub = reinterpret_cast<void*>(executableMatches.front().getAddress());
    }

    const auto ret = blackbase::caller::IndirectCall::call(call_stub, &function, 1, 2, 3, 4, 5);
    std::println("Return value: {}", ret);
}