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
	MemoryHelper();
	MemoryHelper(DWORD pID);
	~MemoryHelper();
	uintptr_t GetModuleBaseAddress(TCHAR* lpszModuleName);
	uintptr_t GetDynamicAddress(uintptr_t baseAddress, vector<int> offsets);
	uintptr_t GetAddressFromSignature(vector<int> signature);
	void SetpID(DWORD pID);
	uintptr_t SetBaseAddress(TCHAR* moduleName);
	DWORD GetpID();
	HANDLE GetprocessHandle();
    uintptr_t GetAddress(uintptr_t AddressOfCall, int index, int length);



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
	DWORD pID;
	uintptr_t pBaseAddress;
	DWORD pSize;
	HANDLE processHandle;
};

#endif// LAZYTP_MEMORYHELPER_H