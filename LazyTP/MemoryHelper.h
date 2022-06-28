#pragma once
#include <Windows.h>  //DWORD, HANDLE
#include <TlHelp32.h> //CreateToolhelp32Snapshot
#include <tchar.h>    //_tcscmp
#include <Psapi.h>    //GetModuleInformation
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
	uintptr_t SetpBaseAddress(TCHAR* moduleName);
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
	uintptr_t pBaseAddress; //Base Address of exe
	DWORD pSize;		//Size of exe module
	HANDLE processHandle;
};