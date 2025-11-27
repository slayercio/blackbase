#pragma once
#include <string>
#include <optional>
#include <vector>
#include <type_traits>
#include <functional>
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/pattern/match.hpp>

namespace blackbase
{
    class Process
    {
    private:
        std::string m_Name;
        int m_ProcessId;

    public:
        inline static Process GetCurrent() noexcept;
        inline static std::optional<Process> FindByName(const std::string& name) noexcept;
        inline static std::optional<Process> FindById(int processId) noexcept;

    public:
        inline const std::string& GetName() const noexcept;
        inline int GetProcessId() const noexcept;    

    public:
        inline std::optional<Match> FindFirst(const Pattern& pattern) const noexcept;
        inline std::vector<Match> FindAll(const Pattern& pattern) const noexcept;
        inline Match& ResolveRelative(Match& match, std::ptrdiff_t offset = Match::LoadEffectiveAddressOffset) const noexcept;

    public:
        inline std::optional<std::vector<std::uint8_t>> ReadMemory(std::uintptr_t address, std::size_t size) const noexcept;
        inline bool WriteMemory(std::uintptr_t address, const std::vector<std::uint8_t>& data) const noexcept;

        template <typename T, typename std::enable_if_t<std::is_trivially_copyable_v<T>, int> = 0>
        inline std::optional<T> ReadMemory(std::uintptr_t address) const noexcept;

        template <typename T, typename std::enable_if_t<std::is_trivially_copyable_v<T>, int> = 0>
        inline bool WriteMemory(std::uintptr_t address, const T& value) const noexcept;

        template <typename T, typename std::enable_if_t<!std::is_trivially_copyable_v<T>, int> = 0>
        inline std::optional<T> ReadMemory(std::uintptr_t address, std::function<T(std::vector<std::uint8_t>&)> converter, size_t size = sizeof(T)) const noexcept;

        template <typename T, typename std::enable_if_t<!std::is_trivially_copyable_v<T>, int> = 0>
        inline bool WriteMemory(std::uintptr_t address, const T& value, std::function<std::vector<std::uint8_t>(const T&)> converter) const noexcept;

        inline std::optional<std::string> ReadString(std::uintptr_t address, std::size_t maxLength = -1) const noexcept;
        inline bool WriteString(std::uintptr_t address, const std::string& value) const noexcept;

    protected:
        Process(const std::string& name, int processId);
        Process(void* handle);

        std::vector<Match> FindInternal(const Pattern& pattern, bool findAll) const noexcept;
    };
}

#pragma region Implementation
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>

namespace blackbase
{
    inline Process Process::GetCurrent() noexcept
    {
        return Process(GetCurrentProcess());
    }

    inline std::optional<Process> Process::FindByName(const std::string& name) noexcept
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return std::nullopt;
        }

        #undef PROCESSENTRY32
        #undef Process32First
        #undef Process32Next

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(snapshot, &entry))
        {
            do
            {
                if (name == entry.szExeFile)
                {
                    CloseHandle(snapshot);
                    return Process(entry.szExeFile, entry.th32ProcessID);
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return std::nullopt;
    }

    inline std::optional<Process> Process::FindById(int processId) noexcept
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return std::nullopt;
        }

        #undef PROCESSENTRY32
        #undef Process32First
        #undef Process32Next

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(snapshot, &entry))
        {
            do
            {
                if (processId == entry.th32ProcessID)
                {
                    CloseHandle(snapshot);
                    return Process(entry.szExeFile, entry.th32ProcessID);
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return std::nullopt;
    }

    inline const std::string& Process::GetName() const noexcept
    {
        return m_Name;
    }

    inline int Process::GetProcessId() const noexcept
    {
        return m_ProcessId;
    }

    inline std::optional<Match> Process::FindFirst(const Pattern& pattern) const noexcept
    {
        const auto matches = FindInternal(pattern, false);
        if (!matches.empty())
        {
            return matches.front();
        }

        return std::nullopt;
    }

    inline std::vector<Match> Process::FindAll(const Pattern& pattern) const noexcept
    {
        return FindInternal(pattern, true);
    }

    inline Match& Process::ResolveRelative(Match& match, std::ptrdiff_t offset) const noexcept
    {
        auto address = match.GetCurrentAddress();
        auto rva = ReadMemory<std::int32_t>(address + offset);
        if (rva)
        {
            match.Move(offset + sizeof(std::int32_t) + *rva);
        }

        return match;
    }

    Process::Process(const std::string& name, int processId)
        : m_Name(name), m_ProcessId(processId)
    {
    }

    Process::Process(void* handle)
    {
        char buffer[MAX_PATH];
        if (GetModuleFileNameA(reinterpret_cast<HMODULE>(handle), buffer, MAX_PATH))
        {
            m_Name = std::string(buffer);
        }

        m_ProcessId = ::GetProcessId(reinterpret_cast<HANDLE>(handle));
    }

    std::vector<Match> Process::FindInternal(const Pattern& pattern, bool findAll) const noexcept
    {
        std::vector<Match> matches;

        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, m_ProcessId);
        if (!hProcess)
        {
            return matches;
        }

        static char buffer[1024];
        SIZE_T bytesRead;
        MEMORY_BASIC_INFORMATION mbi;

        std::uintptr_t address = 0;
        while (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == sizeof(mbi))
        {
            if (mbi.State == MEM_COMMIT && (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_READWRITE)) && !(mbi.Protect & PAGE_GUARD))
            {
                std::uintptr_t regionEnd = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
                std::uintptr_t currentAddress = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);

                while (currentAddress < regionEnd)
                {
                    SIZE_T bytesToRead = std::min(sizeof(buffer), regionEnd - currentAddress);
                    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(currentAddress), buffer, bytesToRead, &bytesRead) && bytesRead > 0)
                    {
                        const auto& patternBytes = pattern.GetBytes();
                        const auto& patternMask = pattern.GetMask();
                        SIZE_T patternSize = patternBytes.size();

                        for (SIZE_T i = 0; i <= bytesRead - patternSize; ++i)
                        {
                            bool found = true;
                            for (SIZE_T j = 0; j < patternSize; ++j)
                            {
                                if (patternMask[j] > 0 && buffer[i + j] != static_cast<char>(patternBytes[j]))
                                {
                                    found = false;
                                    break;
                                }
                            }

                            if (found)
                            {
                                matches.emplace_back(currentAddress + i);
                                if (!findAll)
                                {
                                    CloseHandle(hProcess);
                                    return matches;
                                }
                            }
                        }
                    }

                    currentAddress += bytesRead;
                }
            }

            address += mbi.RegionSize;
        }

        CloseHandle(hProcess);
        return matches;
    }

    inline std::optional<std::vector<std::uint8_t>> Process::ReadMemory(std::uintptr_t address, std::size_t size) const noexcept
    {
        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, m_ProcessId);
        if (!hProcess)
        {
            return std::nullopt;
        }

        DWORD oldProtect;
        if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), size, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            CloseHandle(hProcess);
            return std::nullopt;
        }

        std::vector<std::uint8_t> buffer(size);
        SIZE_T bytesRead;
        if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), buffer.data(), size, &bytesRead) && bytesRead == size)
        {
            CloseHandle(hProcess);
            return buffer;
        }

        VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), size, oldProtect, &oldProtect);
        CloseHandle(hProcess);
        return std::nullopt;
    }

    inline bool Process::WriteMemory(std::uintptr_t address, const std::vector<std::uint8_t>& data) const noexcept
    {
        HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, m_ProcessId);
        if (!hProcess)
        {
            return false;
        }

        DWORD oldProtect;
        if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), data.size(), PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            CloseHandle(hProcess);
            return false;
        }

        SIZE_T bytesWritten;
        if (WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), data.data(), data.size(), &bytesWritten) && bytesWritten == data.size())
        {
            CloseHandle(hProcess);
            return true;
        }

        VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), data.size(), oldProtect, &oldProtect);

        CloseHandle(hProcess);
        return false;
    }

    template <typename T, typename std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    inline std::optional<T> Process::ReadMemory(std::uintptr_t address) const noexcept
    {
        auto bufferOpt = ReadMemory(address, sizeof(T));
        if (bufferOpt && bufferOpt->size() == sizeof(T))
        {
            T value;
            std::memcpy(&value, bufferOpt->data(), sizeof(T));
            return value;
        }
        return std::nullopt;
    }

    template <typename T, typename std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    inline bool Process::WriteMemory(std::uintptr_t address, const T& value) const noexcept
    {
        std::vector<std::uint8_t> data(sizeof(T));
        std::memcpy(data.data(), &value, sizeof(T));
        return WriteMemory(address, data);
    }

    template <typename T, typename std::enable_if_t<!std::is_trivially_copyable_v<T>, int>>
    inline std::optional<T> Process::ReadMemory(std::uintptr_t address, std::function<T(std::vector<std::uint8_t>&)> converter, size_t size) const noexcept
    {
        auto bufferOpt = ReadMemory(address, size);
        if (bufferOpt)
        {
            return converter(*bufferOpt);
        }

        return std::nullopt;
    }

    template <typename T, typename std::enable_if_t<!std::is_trivially_copyable_v<T>, int>>
    inline bool Process::WriteMemory(std::uintptr_t address, const T& value, std::function<std::vector<std::uint8_t>(const T&)> converter) const noexcept
    {
        auto data = converter(value);
        return WriteMemory(address, data);
    }

    inline std::optional<std::string> Process::ReadString(std::uintptr_t address, std::size_t maxLength) const noexcept
    {
        std::string result;
        std::size_t length = 0;

        while (length < maxLength)
        {
            auto charOpt = ReadMemory<char>(address + length);
            if (!charOpt || *charOpt == '\0')
            {
                break;
            }

            result += *charOpt;
            ++length;
        }

        return result;
    }

    inline bool Process::WriteString(std::uintptr_t address, const std::string& value) const noexcept
    {
        std::vector<std::uint8_t> data(value.begin(), value.end());
        data.push_back('\0'); // Null-terminate the string
        return WriteMemory(address, data);
    }
}

#pragma endregion