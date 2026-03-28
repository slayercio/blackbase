#include <blackbase/process.hpp>

int main()
{
    blackbase::Process process = blackbase::Process::GetCurrent();
    std::cout << "Current Process Name: " << process.GetName() << std::endl;
    std::cout << "Current Process ID: " << process.GetProcessId() << std::endl;

    auto testProcess = blackbase::Process::FindByName("modules_tester.exe");
    if (!testProcess.has_value())
    {
        std::cerr << "Could not find modules_tester.exe process." << std::endl;
        return 1;
    }

    std::cout << "Found modules_tester.exe with PID: " << testProcess->GetProcessId() << std::endl;
    std::cout << "Name: " << testProcess->GetName() << std::endl;

    auto modules = testProcess->GetModules();
    std::cout << "Modules loaded in modules_tester.exe:" << std::endl;
    for (const auto& module : modules)
    {
        std::cout << " - " << module.Name << " (Base: " << std::hex << module.BaseAddress << ", End: " << module.EndAddress << std::dec << ")" << std::endl;
    }

    return 0;
}