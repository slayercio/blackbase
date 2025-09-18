#include <blackbase/process.hpp>

int main()
{
    blackbase::Process process = blackbase::Process::GetCurrent();
    std::cout << "Current Process Name: " << process.GetName() << std::endl;
    std::cout << "Current Process ID: " << process.GetProcessId() << std::endl;

    auto testProcess = blackbase::Process::FindByName("rpm_test.exe");
    if (!testProcess.has_value())
    {
        std::cerr << "Could not find rpm_test.exe process." << std::endl;
        return 1;
    }

    std::cout << "Found rpm_test.exe with PID: " << testProcess->GetProcessId() << std::endl;
    std::cout << "Name: " << testProcess->GetName() << std::endl;

    blackbase::Pattern p("48 89 44 ? ? 48 8B 05 ? ? ? ?");
    auto match = testProcess->FindFirst(p);
    if (!match.has_value())
    {
        std::cerr << "Pattern not found in rpm_test.exe." << std::endl;
        return 1;
    }

    std::cout << "Pattern found at address: " << std::hex << match->GetCurrentAddress() << std::dec << std::endl;
    match->Move(5);

    auto rva = testProcess->ResolveRelative(match.value());
    std::cout << "Resolved relative address: " << std::hex << rva.GetCurrentAddress() << std::dec << std::endl;
    
    auto ptr = testProcess->ReadMemory<std::uintptr_t>(rva.GetCurrentAddress());
    if (ptr.has_value())
    {
        std::cout << "Pointer value at resolved address: " << std::hex << *ptr << std::dec << std::endl;
    }
    else
    {
        std::cerr << "Failed to read pointer at resolved address." << std::endl;
    }

    auto memory = testProcess->ReadMemory(*ptr, 0x10);
    if (memory.has_value())
    {
        auto str = testProcess->ReadString(*ptr);
        if (str.has_value())
        {
            std::cout << "String at pointer: " << *str << std::endl;
        
            std::string overwrite = "Hello from Blackbone!";
            if (testProcess->WriteString(*ptr, overwrite))
            {
                std::cout << "Successfully wrote string to memory." << std::endl;
            }
            else
            {
                std::cerr << "Failed to write string to memory." << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to read string at pointer." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to read memory at match address." << std::endl;
    }

    return 0;
}