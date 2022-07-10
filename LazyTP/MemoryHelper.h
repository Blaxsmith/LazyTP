#ifndef LAZYTP_MEMORYHELPER_H
#define LAZYTP_MEMORYHELPER_H

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <Psapi.h>
#include <vector>
#include <optional>
using namespace std;

class MemoryHelper
{
public:
	MemoryHelper(DWORD pID, const wchar_t* moduleName);
	~MemoryHelper();
	uintptr_t GetAddressFromSignature(vector<unsigned char> signature);
	uintptr_t GetBaseAddress();
    uintptr_t GetAddress(uintptr_t AddressOfCall, int index, int length);
    [[nodiscard]] uintptr_t GetAddressWithOffsets(uintptr_t baseAddress, const vector<unsigned short>& offsets, bool baseAddressIsPointer = false) const;

private:
    uintptr_t GetModuleBaseAddress(TCHAR* lpszModuleName);

public:
    template<typename T>
    [[nodiscard]] std::optional<T> Read(const std::uintptr_t address_ptr) const noexcept(false)
    {
        T length;

        if ((!ReadProcessMemory(processHandle, reinterpret_cast<void*>(address_ptr), std::addressof(length), sizeof(length), nullptr)))
        {
            return std::nullopt;
        }

        return std::optional<T>(std::move(length));
    }



    template<typename T>
    bool Write(const std::uintptr_t address_ptr, const T& length) const noexcept
    {
        return WriteProcessMemory(processHandle, reinterpret_cast<void*>(address_ptr), std::addressof(length), sizeof(length), nullptr);
    }
	
private:
	DWORD processID;
	uintptr_t pBaseAddress;
	DWORD processSize;
	HANDLE processHandle;
};

#endif// LAZYTP_MEMORYHELPER_H